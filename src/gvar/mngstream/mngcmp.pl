#! /usr/local/bin/perl
#---------------------------------------------------------------------
#
#	MNG�R���p�C���X�N���v�g Ver.1.1 (2003/ 1/31)
#	(c) 2001-2003 ����
#	http://www.aurora.dti.ne.jp/~zom/Counter/mng/compiler/
#
#---------------------------------------------------------------------
#
#	���̃X�N���v�g�̍Ĕz�z�E���ς́A��L�̒��쌠�\����������
#	���莩�R�ł��B
#	�܂��A���̃X�N���v�g�𑼂̃v���O���~���O�����X�N���v�g
#	����ɈڐA���邱�Ƃɂ��ẮA���p���p���s��ʏꍇ�Ɍ���A
#	���R�ɍs���܂��B
#
#---------------------------------------------------------------------


# ���ϐ��B
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


# �`�����N�o�̓��[�`���B
sub writeChunk{

	local(*chunkData) = @_;
	my($crc) = 0xffffffff;

	print OUT pack('N', length($chunkData) - 4).$chunkData;
	foreach(unpack('C*', $chunkData)){
		$crc = $crcTable[($crc ^ $_) & 0xff] ^ ($crc >> 8);
	}
	print OUT pack('N', ~$crc);

}


# ���\�[�X�t�@�C�������B
sub readRes{

	local($resFile, $mngFile) = @_;
	local($line, @res);
	my($resFileName);

	if(!open(IN, $resFile)){
		&error("$resFile�̃I�[�v���Ɏ��s���܂����B");
		return 1;
	}
	@res = <IN>;
	close(IN);

	if($resFile =~ /^(.*[\/\\])(.*)$/){
		$resFileName = $2;
		if(!chdir($1)){		# �ǂ߂��Ƃ������Ƃ̓f�B���N�g���͂���͂��B
			&error("�f�B���N�g���̈ړ��Ɏ��s���܂����B");
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
		&error("$mngFile�̃I�[�v���Ɏ��s���܂����B");
		return 2;
	}

	print qq|"$resFile" ��������...\n|;
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
	print qq|"$mngFile" �Ƃ������O�ŏo�͂��܂����B\n|;
	print "OK\n";

	return 0;

}


# �e�p�����[�^�𓾂�B
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


# �s���p�[�X����B
sub parseLine{

	local(*line) = @_;
	my($chunkName, $numOfParams, $i);
	local($header, $chunkData, $chunkOrder, @param) = ('', '', '', ());

	if($line eq '' || $line eq "\n"){ return 0; }

	&divideLine($line, *param);
	$chunkName   = $param[0];
	$numOfParams = $#param;
	return 0 if($numOfParams < 0);

	$errMsg = '�p�����[�^�̐�����������';

	return &simpleParse(*param, $numOfParams) if($simpleModeStatus >= 1);

	if($chunkName =~ /^(....):(.*)/){	# �v���C�x�[�g�`�����N�B
		$chunkName  = $1;
		$chunkOrder = $2;
		if($chunkOrder =~ /[^124fkKt]/){
			$errMsg = '�`�����N�\����\��������ɕςȕ������������Ă���';
			return 1;
		}
		if(length($chunkOrder) != $numOfParams){
			$errMsg = '�`�����N�\����\��������ƃp�����[�^�̐��Ƃ����v����';
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

	# �ȉ�PNG�`�����N�B

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
		$errMsg = '���Ή��̃`�����N�܂��͖����ȕ�����';
		return 1;
	}

	if(length($chunkOrder) != $numOfParams){
		$errMsg  = "�����G���[�E$chunkName�̏����Ƀo�O������";
		return 1;
	}

	# chunkOrder���Ƀ`�����N���\�z�B
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
				$errMsg = "$i�Ԗڂ̃p�����[�^��80�����ȏ゠��";
				return 1;
			}
			if($_ eq 'k' || $_ eq 't' || $param[$i] ne '\0'){
				$chunkData .= $param[$i];
			}
			if($i < $numOfParams){	# �܂�����B
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


# ���S�Œ蒷�̃`�����N�B
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


# �L���ŉϒ��̃`�����N�B
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


# �����̉ϒ��`�����N�B
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


# ����`�����N�B
sub FRAMChunk{

	local(*param, $numOfParams, *chunkOrder) = @_;
	my($orderLength);

	$chunkOrder = '1K1111';
	if($numOfParams >= 0 && $numOfParams <= 2){	# 0-2��OK.
		$chunkOrder = substr($chunkOrder, 0, $numOfParams);
		return 0;
	}

	if($numOfParams < 6){ return 1; }	# 3-5�͂�����B

	if($param[3] != 0){ $chunkOrder .= '4'; }
	if($param[4] != 0){ $chunkOrder .= '4'; }
	if($param[5] != 0){ $chunkOrder .= '14444'; }

	$orderLength = length($chunkOrder);

	if($numOfParams < $orderLength){ return 1; }	# �p�����[�^�s���B
	if($param[6] == 0 && $numOfParams != $orderLength){ return 1; }	# �����B

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
			$errMsg  = '���̃X�N���v�g�̐����ɂ��ASAVE�`�����N��';
			$errMsg .= '�ŏ��̃p�����[�^��4�̂ݎw���';
			return 1;
		}
		else{
			$errMsg  = 'SAVE�`�����N�̍ŏ��̃p�����[�^��4�̂�';
			$errMsg .= '�i�d�l���8���j�w���';
			return 1;
		}
	}
	$chunkOrder = '1';
	if($numOfParams == 1){ return 0; }

	for($cl = 1; $cl < $numOfParams; $cl++){	# entry_type�����ǂ�ł䂭�B
		$left = $numOfParams - $cl;
		if($param[$cl] == 0){	# entry, ofs, st_tm, la_num, fr_num, (name).
			if($left < 5){		# �����B
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
			if($left < 2){		# �����B
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
			if($left < 1){		# ���蓾�񂯂ǁB
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


# ���̂܂܁B
sub xHDRChunk{		# IHDR/JHDR.

	local(*param, $numOfParams, *header) = @_;
	my($pngHeader, $chunkData);

	if($numOfParams != 1){ return 1; }
	if(!open(PNG, $param[1])){
		$errMsg = "$param[1]�̃I�[�v���Ɏ��s";
		return 1;
	}
	binmode(PNG);
	seek(PNG, 0, 0);
	read(PNG, $pngHeader, 8);
	read(PNG, $chunkData, -s $param[1]);
	close(PNG);

	if($pngHeader ne $header){
		$errMsg  = "$param[1]��";
		$errMsg .= substr($header, 1, 3);
		$errMsg .= "�摜�ł͂Ȃ�";
		return 1;
	}
	print OUT $chunkData;

	return 0;

}


# �t�@�C���f�[�^��ǂ�ŏo�́B
sub readFileAsChunk{

	local($fileName, *chunkData) = @_;
	my($fileData);

	if(!open(CHUNKFILE, $fileName)){
		$errMsg = "$fileName�̃I�[�v���Ɏ��s";
		return 1;
	}
	binmode(CHUNKFILE);
	seek(CHUNKFILE, 0, 0);
	read(CHUNKFILE, $fileData, -s $fileName);
	close(CHUNKFILE);

	$chunkData .= $fileData;

	return 0;

}


# CRC�e�[�u���쐬�B
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


# �ȈՃ��[�h�p�������B
sub simpleParse{

	local(*param, $numOfParams) = @_;
	local($header);
	my($comName);

	$comName = $param[0];
	return &simpleName(*param, $numOfParams)   if($comName eq 'name');
	return &simpleSize(*param, $numOfParams)   if($comName eq 'size');
	return &simpleRepeat(*param, $numOfParams) if($comName eq 'repeat');
	if($simpleModeStatus < 3){
		$errMsg = 'name, size, repeat�̂��Âꂩ���Ȃ�';
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

	$errMsg = '���Ή��̃R�}���h���܂��͖����ȕ�����';
	return 1;

}


sub simpleName{

	local(*param, $numOfParams) = @_;

	if($lineNo != 1){
		$errMsg = 'name���߂�1�s�ڈȊO�ɂ���';
		return 1;
	}
	return 0;

}


sub simpleSize{

	local(*param, $numOfParams) = @_;
	local($chunkData);

	if($simpleModeStatus < 1){
		$errMsg = '1�s�ڂ�name���߂��Ȃ�';
		return 1;
	}
	if($lineNo != 2){
		$errMsg = 'size���߂�2�s�ڈȊO�ɂ���';
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
		$errMsg = '2�s�ڂ�size���߂��Ȃ�';
		return 1;
	}
	if($lineNo != 3){
		$errMsg = 'repeat���߂�3�s�ڈȊO�ɂ���';
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


# �G���[�����B
sub error{
	print "$_[0]\n";
}

sub syntaxError{
	print "$lineNo�s�ڂŃG���[���������܂����B\n";
	print "($errMsg)\n";
	print "$_[0]\n";
}
#---------------------------------------------------------------------
#
#	MNG�R���p�C���X�N���v�g��������
#
#		Ver.0.1 (2001/12/22)
#
#		Ver.0.2 (2001/12/23)
#			�o�͂���MNG�t�@�C���������\�[�X�t�@�C������
#			�w��ł���悤�ɂ���(�����������ł̎w����
#			�R�}���h���C���ł̎w��̂ق����D�悷��)�B
#
#		Ver.0.21 (2001/12/23)
#			LOOP�`�����N�����̃o�O���C���B
#
#		Ver.0.22 (2001/12/23)
#			SAVE�`�����N�����̉��ߊԈႢ���C���B
#
#		Ver.0.3 (2001/12/24)
#			PNG�֘A�`�����N��ǉ��B
#			SAVE�`�����N�����̉��ߊԈႢ���ďC���B
#
#		Ver.1.0beta1 (2002/10/27)
#			�`�����N������啝���������B���I�ȃ`�����N���D�荞�߂��
#			���ɁB
#			FRAM�`�����N��sync_id��1����������ʂƂ����o�O������
#			���̂ŏC���B
#			���̃X�N���v�g�ƈႤ�f�B���N�g���ɂ��郊�\�[�X���R���p�C
#			�����悤�Ƃ���ƁA�J�����g�������������C���B
#
#		Ver.1.0beta2 (2002/11/21)
#			�o�͂����MNG�̊���̖��O���A"���\�[�X�t�@�C��.mng" �ɁB
#
#		Ver.1.0 (2002/12/31)
#			�����Ń����[�X�B���g�̓�2�Ɠ����B
#
#		Ver.1.1 (2003/ 1/31)
#			�ȈՕ����Ή��@�\��t���B
#
#---------------------------------------------------------------------
