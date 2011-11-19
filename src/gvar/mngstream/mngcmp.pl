#! /usr/local/bin/perl
#---------------------------------------------------------------------
#
#	MNGコンパイラスクリプト Ver.1.1 (2003/ 1/31)
#	(c) 2001-2003 桜月
#	http://www.aurora.dti.ne.jp/~zom/Counter/mng/compiler/
#
#---------------------------------------------------------------------
#
#	このスクリプトの再配布・改変は、上記の著作権表示を消さぬ
#	限り自由です。
#	また、このスクリプトを他のプログラミング言語やスクリプト
#	言語に移植することについては、商用利用を行わぬ場合に限り、
#	自由に行えます。
#
#---------------------------------------------------------------------


# 大域変数。
@crcTable = ();
$lineNo   = 0;
$errMsg   = '';
$simpleModeStatus = 0;


exit(&main());


# main.
sub main{

	&makeCrcTable();
	return &readRes($ARGV[0] || 'mngres.txt', $ARGV[1]);

}


# チャンク出力ルーチン。
sub writeChunk{

	local(*chunkData) = @_;
	my($crc) = 0xffffffff;

	print OUT pack('N', length($chunkData) - 4).$chunkData;
	foreach(unpack('C*', $chunkData)){
		$crc = $crcTable[($crc ^ $_) & 0xff] ^ ($crc >> 8);
	}
	print OUT pack('N', ~$crc);

}


# リソースファイル処理。
sub readRes{

	local($resFile, $mngFile) = @_;
	local($line, @res);
	my($resFileName);

	if(!open(IN, $resFile)){
		&error("$resFileのオープンに失敗しました。");
		return 1;
	}
	@res = <IN>;
	close(IN);

	if($resFile =~ /^(.*[\/\\])(.*)$/){
		$resFileName = $2;
		if(!chdir($1)){		# 読めたということはディレクトリはあるはず。
			&error("ディレクトリの移動に失敗しました。");
			return 4;
		}
	}
	else{
		$resFileName = $resFile;
	}

	$simpleModeStatus = 0;
	if(!$mngFile){
		if($res[0] =~ /^#\s*?MNG\s*=\s*(.+)/i){
			$mngFile = $1;
		}
		elsif($res[0] =~ /^name\s*?(.+)/i){
			$mngFile = $1;
			$simpleModeStatus = 1;
		}
		else{
			if($resFileName =~ /^(.+)\.txt$/i){
				$mngFile = "$1.mng";
			}
			else{
				$mngFile = "$resFileName.mng";
			}
		}
	}

	if(!open(OUT, ">$mngFile")){
		&error("$mngFileのオープンに失敗しました。");
		return 2;
	}

	print qq|"$resFile" を処理中...\n|;
	binmode(OUT);
	print OUT "\x8aMNG\x0d\x0a\x1a\x0a";

	foreach $line (@res){
		chomp($line);
		$lineNo++;
		if(&parseLine(*line)){
			&syntaxError($line);
			close(OUT);
			return 3;
		}
	}

	close(OUT);
	print qq|"$mngFile" という名前で出力しました。\n|;
	print "OK\n";

	return 0;

}


# 各パラメータを得る。
sub divideLine{

	local($line, *param) = @_;
	my($fMustRepeat, $value);

	@param = ();
	if($line =~ /^\s+(.*)/){
		$line = $1;
	}
	$line .= " dummy";
	do{
		$fMustRepeat = 0;
		if($line =~ /^(.+?)\s+(.+)$/){
			$value = $1;
			$line  = $2;
			if(substr($value, 0, 1) eq '#'){ return; }
			$fMustRepeat = 1;
			if(substr($value, 0, 2) eq '0x'){
				$value = hex($value);
			}
			elsif(substr($value, 0, 1) eq '0'){
				$value = oct($value);
			}
			push(@param, $value);
		}
	}
	while($fMustRepeat);

}


# 行をパースする。
sub parseLine{

	local(*line) = @_;
	my($chunkName, $numOfParams, $i);
	local($header, $chunkData, $chunkOrder, @param) = ('', '', '', ());

	if($line eq '' || $line eq "\n"){ return 0; }

	&divideLine($line, *param);
	$chunkName   = $param[0];
	$numOfParams = $#param;
	return 0 if($numOfParams < 0);

	$errMsg = 'パラメータの数がおかしい';

	return &simpleParse(*param, $numOfParams) if($simpleModeStatus >= 1);

	if($chunkName =~ /^(....):(.*)/){	# プライベートチャンク。
		$chunkName  = $1;
		$chunkOrder = $2;
		if($chunkOrder =~ /[^124fkKt]/){
			$errMsg = 'チャンク構造を表す文字列に変な文字が混じっている';
			return 1;
		}
		if(length($chunkOrder) != $numOfParams){
			$errMsg = 'チャンク構造を表す文字列とパラメータの数とが合致せず';
			return 1;
		}
	}
	elsif($chunkName eq 'MHDR'){
		if(&MHDRChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'MEND'){
		if(&MENDChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'LOOP'){
		if(&LOOPChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'ENDL'){
		if(&ENDLChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'DEFI'){
		if(&DEFIChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
#	elsif($chunkName eq 'PLTE'){
#		if(&PLTEChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
#	}
#	elsif($chunkName eq 'tRNS'){
#		if(&tRNSChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
#	}
	elsif($chunkName eq 'IHDR'){
		$header = "\x89PNG\x0d\x0a\x1a\x0a";
		return &xHDRChunk(*param, $numOfParams, *header);
	}
	elsif($chunkName eq 'JHDR'){
		$header = "\x8bJNG\x0d\x0a\x1a\x0a";
		return &xHDRChunk(*param, $numOfParams, *header);
	}
#	elsif($chunkName eq 'BASI'){
#		if(&BASIChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
#	}
	elsif($chunkName eq 'CLON'){
		if(&CLONChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
#	elsif($chunkName eq 'DHDR'){
#		if(&DHDRChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
#	}
	elsif($chunkName eq 'PAST'){
		if(&PASTChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'MAGN'){
		if(&MAGNChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'DISC'){
		if(&DISCChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'TERM'){
		if(&TERMChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'BACK'){
		if(&BACKChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'FRAM'){
		if(&FRAMChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'MOVE'){
		if(&MOVEChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'CLIP'){
		if(&CLIPChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'SHOW'){
		if(&SHOWChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'SAVE'){
		if(&SAVEChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'SEEK'){
		if(&SEEKChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'eXPI'){
		if(&eXPIChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'fPRI'){
		if(&fPRIChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'nEED'){
		if(&nEEDChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'pHYg'){
		if(&pHYgChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}

	# 以下PNGチャンク。

#	elsif($chunkName eq 'iTXt'){
#		if(&iTXtChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
#	}
	elsif($chunkName eq 'tEXt'){
		if(&tEXtChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
#	elsif($chunkName eq 'zTXt'){
#		if(&zTXtChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
#	}
	elsif($chunkName eq 'tIME'){
		if(&tIMEChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'cHRM'){
		if(&cHRMChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
	elsif($chunkName eq 'gAMA'){
		if(&gAMAChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
#	elsif($chunkName eq 'iCCP'){
#		if(&iCCPChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
#	}
	elsif($chunkName eq 'sRGB'){
		if(&sRGBChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
#	elsif($chunkName eq 'bKGD'){
#		if(&bKGDChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
#	}
#	elsif($chunkName eq 'sBIT'){
#		if(&sBITChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
#	}
	elsif($chunkName eq 'pHYs'){
		if(&pHYgChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
	}
#	elsif($chunkName eq 'sPLT'){
#		if(&sPLTChunk(*param, $numOfParams, *chunkOrder)){ return 1; }
#	}

	else{
		$errMsg = '未対応のチャンクまたは無効な文字列';
		return 1;
	}

	if(length($chunkOrder) != $numOfParams){
		$errMsg  = "内部エラー・$chunkNameの処理にバグがある";
		return 1;
	}

	# chunkOrder順にチャンクを構築。
	$i = 1;
	$chunkData  = $chunkName;
	$chunkOrder =~ tr/124/CnN/;
	foreach(split(//, $chunkOrder)){
		if(
			$_ eq 'k'	# 1-79
			||
			$_ eq 't'	# 1-
			||
			$_ eq 'K'	# 0-79
			){
			if(($_ eq 'k' || $_ eq 'K') && (length($param[$i]) > 79)){
				$errMsg = "$i番目のパラメータが80文字以上ある";
				return 1;
			}
			if($_ eq 'k' || $_ eq 't' || $param[$i] ne '\0'){
				$chunkData .= $param[$i];
			}
			if($i < $numOfParams){	# まだある。
				$chunkData .= "\0";
			}
		}
		elsif($_ eq 'f'){
			if(&readFileAsChunk($param[$i], *chunkData)){
				return 1;
			}
		}
		else{
			$chunkData .= pack($_, $param[$i]);
		}
		$i++;
	}

	writeChunk(*chunkData);
	return 0;

}


# 完全固定長のチャンク。
sub MHDRChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams != 7){ return 1; }
	$chunkOrder = '4444444';

	return 0;

}


sub MENDChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams != 0){ return 1; }
	$chunkOrder = '';

	return 0;
}


sub ENDLChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams != 1){ return 1; }
	$chunkOrder = '1';

	return 0;

}


sub MOVEChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams != 5){ return 1; }
	$chunkOrder = '22144';

	return 0;

}


sub CLIPChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams != 7){ return 1; }
	$chunkOrder = '2214444';

	return 0;

}


sub eXPIChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams != 2){ return 1; }
	$chunkOrder = '2k';

	return 0;

}


sub fPRIChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams != 2){ return 1; }
	$chunkOrder = '11';

	return 0;

}


sub pHYgChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams != 3){ return 1; }
	$chunkOrder = '441';

	return 0;

}


sub tEXtChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams != 2){ return 1; }
	$chunkOrder = 'kt';

	return 0;

}


sub tIMEChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams != 6){ return 1; }
	$chunkOrder = '211111';

	return 0;

}


sub cHRMChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams != 8){ return 1; }
	$chunkOrder = '44444444';

	return 0;

}


sub gAMAChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams != 1){ return 1; }
	$chunkOrder = '4';

	return 0;

}


sub sRGBChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams != 1){ return 1; }
	$chunkOrder = '1';

	return 0;

}


# 有限で可変長のチャンク。
sub DEFIChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if(
		$numOfParams == 1	# 2bytes.
		||
		$numOfParams == 2	# 3bytes.
		||
		$numOfParams == 3	# 4bytes.
		||
		$numOfParams == 5	# 12bytes.
		||
		$numOfParams == 9	# 28bytes.
		){
		$chunkOrder = substr('211444444', 0, $numOfParams);
		return 0;
	}
	return 1;

}


sub CLONChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if(
		$numOfParams == 2	# 4bytes.
		||
		$numOfParams == 3	# 5bytes.
		||
		$numOfParams == 4	# 6bytes.
		||
		$numOfParams == 5	# 7bytes.
		||
		$numOfParams == 8	# 16bytes.
		){
		$chunkOrder = substr('22111144', 0, $numOfParams);
		return 0;
	}
	return 1;

}


sub MAGNChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams >= 0 && $numOfParams <= 10){
		$chunkOrder = substr('2212222221', 0, $numOfParams);
		return 0;
	}
	return 1;

}


sub TERMChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams == 1 || $numOfParams == 4){	# 1byte/10bytes.
		$chunkOrder = substr('1144', 0, $numOfParams);
		return 0;
	}
	return 1;

}


sub BACKChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams >= 3 && $numOfParams <= 6){	# 6, 7, 9, 10bytes.
		$chunkOrder = substr('222121', 0, $numOfParams);
		return 0;
	}
	return 1;

}


sub SHOWChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams >= 0 && $numOfParams <= 3){	# 0, 2, 4, 5bytes.
		$chunkOrder = substr('221', 0, $numOfParams);
		return 0;
	}
	return 1;

}


sub SEEKChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams >= 0 && $numOfParams <= 1){
		$chunkOrder = substr('K', 0, $numOfParams);
		return 0;
	}
	return 1;

}


# 無限の可変長チャンク。
sub LOOPChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams == 2){
		$chunkOrder = '14';
	}
	elsif($numOfParams >= 3){
		$chunkOrder  = '141';
		$chunkOrder .= '4' x ($numOfParams - 3);
	}
	else{ return 1; }

	return 0;

}


sub PASTChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams < 15){ return 1; }
	$chunkOrder  = '2144';
	$numOfParams -= 4;

	if($numOfParams % 11){ return 1; }
	$numOfParams /= 11;
	$chunkOrder .= '21114414444' x $numOfParams;

	return 0;

}


sub DISCChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	$chunkOrder = '2' x $numOfParams;

	return 0;

}


sub nEEDChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;

	if($numOfParams <= 0){ return 1; }
	$chunkOrder = 'k' x $numOfParams;

	return 0;

}


# 特殊チャンク。
sub FRAMChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;
	my($orderLength);

	$chunkOrder = '1K1111';
	if($numOfParams >= 0 && $numOfParams <= 2){	# 0-2はOK.
		$chunkOrder = substr($chunkOrder, 0, $numOfParams);
		return 0;
	}

	if($numOfParams < 6){ return 1; }	# 3-5はあかん。

	if($param[3] != 0){ $chunkOrder .= '4'; }
	if($param[4] != 0){ $chunkOrder .= '4'; }
	if($param[5] != 0){ $chunkOrder .= '14444'; }

	$orderLength = length($chunkOrder);

	if($numOfParams < $orderLength){ return 1; }	# パラメータ不足。
	if($param[6] == 0 && $numOfParams != $orderLength){ return 1; }	# 多い。

	$chunkOrder .= '4' x ($numOfParams - $orderLength);

	return 0;

}


sub SAVEChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;
	my($offsetSize, $entryType, $cl, $left);

	if($numOfParams == 0){
		$chunkOrder = '';
		return 0;
	}

	$offsetSize = $param[1];
	if($offsetSize != 4){
		if($offsetSize == 8){
			$errMsg  = 'このスクリプトの制限により、SAVEチャンクの';
			$errMsg .= '最初のパラメータは4のみ指定可';
			return 1;
		}
		else{
			$errMsg  = 'SAVEチャンクの最初のパラメータは4のみ';
			$errMsg .= '（仕様上は8も）指定可';
			return 1;
		}
	}
	$chunkOrder = '1';
	if($numOfParams == 1){ return 0; }

	for($cl = 1; $cl < $numOfParams; $cl++){	# entry_typeだけ読んでゆく。
		$left = $numOfParams - $cl;
		if($param[$cl] == 0){	# entry, ofs, st_tm, la_num, fr_num, (name).
			if($left < 5){		# 足らん。
				return 1;
			}
			elsif($left == 5){
				$chunkOrder .= '14444';
				$cl += 4;
			}
			else{
				$chunkOrder .= '14444K';
				$cl += 5;
			}
		}
		elsif($param[$cl] == 1){	# entry, ofs, (name).
			if($left < 2){		# 足らん。
				return 1;
			}
			elsif($left == 2){
				$chunkOrder .= '14';
				$cl += 1;
			}
			else{
				$chunkOrder .= '14K';
				$cl += 2;
			}
		}
		else{					# entre_type >= 2, entry, (name).
			if($left < 1){		# あり得んけど。
				return 1;
			}
			elsif($left == 1){
				$chunkOrder .= '1';
			}
			else{
				$chunkOrder .= '1K';
				$cl++;
			}
		}
	}
	return 0;

}


# そのまま。
sub xHDRChunk{		# IHDR/JHDR.

	local(*param, $numOfParams, *header) = @_;
	my($pngHeader, $chunkData);

	if($numOfParams != 1){ return 1; }
	if(!open(PNG, $param[1])){
		$errMsg = "$param[1]のオープンに失敗";
		return 1;
	}
	binmode(PNG);
	seek(PNG, 0, 0);
	read(PNG, $pngHeader, 8);
	read(PNG, $chunkData, -s $param[1]);
	close(PNG);

	if($pngHeader ne $header){
		$errMsg  = "$param[1]は";
		$errMsg .= substr($header, 1, 3);
		$errMsg .= "画像ではない";
		return 1;
	}
	print OUT $chunkData;

	return 0;

}


# ファイルデータを読んで出力。
sub readFileAsChunk{

	local($fileName, *chunkData) = @_;
	my($fileData);

	if(!open(CHUNKFILE, $fileName)){
		$errMsg = "$fileNameのオープンに失敗";
		return 1;
	}
	binmode(CHUNKFILE);
	seek(CHUNKFILE, 0, 0);
	read(CHUNKFILE, $fileData, -s $fileName);
	close(CHUNKFILE);

	$chunkData .= $fileData;

	return 0;

}


# CRCテーブル作成。
sub makeCrcTable{

	my($crc, $i, $j);

	for($i = 0; $i < 256; $i++){
		$crc = $i;
		for($j = 0; $j < 8; $j++){
			if($crc & 1){ $crc = 0xedb88320 ^ ($crc >> 1); }
			else{ $crc = $crc >> 1; }
		}
		$crcTable[$i] = $crc;
	}

}


# 簡易モード用処理部。
sub simpleParse{

	local(*param, $numOfParams) = @_;
	local($header);
	my($comName);

	$comName = $param[0];
	return &simpleName(*param, $numOfParams)   if($comName eq 'name');
	return &simpleSize(*param, $numOfParams)   if($comName eq 'size');
	return &simpleRepeat(*param, $numOfParams) if($comName eq 'repeat');
	if($simpleModeStatus < 3){
		$errMsg = 'name, size, repeatのいづれかがない';
		return 1;
	}
	return &simplePos(*param, $numOfParams)    if($comName eq 'pos');
	return &simpleFrame(*param, $numOfParams)  if($comName eq 'frame');
	return &simpleBack(*param, $numOfParams)   if($comName eq 'back');
	return &simpleMode(*param, $numOfParams)   if($comName eq 'mode');
	if($comName eq 'png'){
		$header = "\x89PNG\x0d\x0a\x1a\x0a";
		return &xHDRChunk(*param, $numOfParams, *header);
	}
	if($comName eq 'jng'){
		$header = "\x8bJNG\x0d\x0a\x1a\x0a";
		return &xHDRChunk(*param, $numOfParams, *header);
	}
	return &simpleEnd(*param, $numOfParams)    if($comName eq 'end');

	$errMsg = '未対応のコマンド名または無効な文字列';
	return 1;

}


sub simpleName{

	local(*param, $numOfParams) = @_;

	if($lineNo != 1){
		$errMsg = 'name命令が1行目以外にある';
		return 1;
	}
	return 0;

}


sub simpleSize{

	local(*param, $numOfParams) = @_;
	local($chunkData);

	if($simpleModeStatus < 1){
		$errMsg = '1行目にname命令がない';
		return 1;
	}
	if($lineNo != 2){
		$errMsg = 'size命令が2行目以外にある';
		return 1;
	}

	return 1 if($numOfParams != 2);
	$chunkData = 'MHDR'.pack('N7', $param[1], $param[2], 1000, 0, 0, 0, 0x1db);

	writeChunk(*chunkData);
	$simpleModeStatus = 2;
	return 0;

}


sub simpleRepeat{

	local(*param, $numOfParams) = @_;
	local($chunkData);

	if($simpleModeStatus < 2){
		$errMsg = '2行目にsize命令がない';
		return 1;
	}
	if($lineNo != 3){
		$errMsg = 'repeat命令が3行目以外にある';
		return 1;
	}

	return 1 if($numOfParams != 1);
	$chunkData = 'TERM'.pack('C2N', 3, 0, 0);
	if($param[1] eq '*'){
		$chunkData .= pack('N', 0x7fffffff);
	}
	else{
		$chunkData .= pack('N', $param[1]);
	}

	writeChunk(*chunkData);
	$simpleModeStatus = 3;
	return 0;

}


sub simplePos{

	local(*param, $numOfParams) = @_;
	local($chunkData);

	return 1 if($numOfParams != 2);
	$chunkData = 'DEFI'.pack('nC2N2', 0, 0, 0, $param[1], $param[2]);

	writeChunk(*chunkData);
	return 0;

}


sub simpleFrame{

	local(*param, $numOfParams) = @_;
	local($chunkData);

	return 1 if($numOfParams != 4);
	$chunkData = 'DEFI'.pack('nC2N2N4', 0, 0, 0, 0, 0,
		$param[1], $param[2], $param[3], $param[4]);

	writeChunk(*chunkData);
	return 0;

}


sub simpleBack{

	local(*param, $numOfParams) = @_;
	local($chunkData);

	return 1 if($numOfParams != 3);
	$chunkData = 'BACK'.pack('n3C', $param[1], $param[2], $param[3], 1);

	writeChunk(*chunkData);
	return 0;

}


sub simpleMode{

	local(*param, $numOfParams) = @_;
	local($chunkData);

	return 1 if($numOfParams >= 3);
	$chunkData = 'FRAM';
	if($numOfParams >= 1){
		$chunkData .= pack('C', $param[1]);
		if($numOfParams >= 2){
			$chunkData .= pack('C5N', 0, 2, 0, 0, 0, $param[2]);
		}
	}

	writeChunk(*chunkData);
	return 0;

}


sub simpleEnd(){

	local(*param, $numOfParams) = @_;
	local($chunkData);

	return 1 if($numOfParams != 0);
	$chunkData = 'MEND';

	writeChunk(*chunkData);
	return 0;

}


# エラー処理。
sub error{
	print "$_[0]\n";
}

sub syntaxError{
	print "$lineNo行目でエラーが発生しました。\n";
	print "($errMsg)\n";
	print "$_[0]\n";
}
#---------------------------------------------------------------------
#
#	MNGコンパイルスクリプト改訂履歴
#
#		Ver.0.1 (2001/12/22)
#
#		Ver.0.2 (2001/12/23)
#			出力するMNGファイル名をリソースファイル中で
#			指定できるようにする(ただしここでの指定より
#			コマンドラインでの指定のほうが優先する)。
#
#		Ver.0.21 (2001/12/23)
#			LOOPチャンク処理のバグを修正。
#
#		Ver.0.22 (2001/12/23)
#			SAVEチャンク処理の解釈間違いを修正。
#
#		Ver.0.3 (2001/12/24)
#			PNG関連チャンクを追加。
#			SAVEチャンク処理の解釈間違いを再修正。
#
#		Ver.1.0beta1 (2002/10/27)
#			チャンク処理を大幅書き換え。私的なチャンクも織り込めるよ
#			うに。
#			FRAMチャンクにsync_idを1つしか入れられぬというバグがあっ
#			たので修正。
#			このスクリプトと違うディレクトリにあるリソースをコンパイ
#			ルしようとすると、カレントを見失う問題を修正。
#
#		Ver.1.0beta2 (2002/11/21)
#			出力されるMNGの既定の名前を、"リソースファイル.mng" に。
#
#		Ver.1.0 (2002/12/31)
#			正式版リリース。中身はβ2と同じ。
#
#		Ver.1.1 (2003/ 1/31)
#			簡易方式対応機能を付加。
#
#---------------------------------------------------------------------
