#include <cstdlib>
#include <cstdio>

// this function reverses the bits of a byte
static unsigned char reverse_bits(unsigned char byte1) {       
	unsigned char byte2 = 0;
	for ( unsigned i = 0; i < 8; i++ ) {
		int bit_pos_in = 1 << i ;
		int bit_pos_ou = 1 << (8-1 - i);
		if ( (byte1 & bit_pos_in) == bit_pos_in )
			byte2 = byte2 | bit_pos_ou; 
	}
	return byte2;
}

// this function prints the bits of a byte
static void print_bits(unsigned char byte1, char* out) {       
	for ( unsigned i = 0; i < 8; i++ ) {
		int bit_pos_in = 1 << i ;
		if ( (byte1 & bit_pos_in) == bit_pos_in )
			out[8-1 -i] = '1';
		else
			out[8-1 -i] = '0'; 
	}
	out[8] = 0;
}

int main(int argc, char** argv) {
	if ( argc < 2 ) {
		printf("reverse_bits byte\n");
		return 1;
	}
	unsigned char byte1 = (unsigned char) atoi(argv[1]); 
	unsigned char byte2 = reverse_bits(byte1);

	char bits1[16];	
	char bits2[16];
	print_bits(byte1, bits1);	
	print_bits(byte2, bits2);	
	printf("byte1 = %02x %s\n", byte1, bits1);
	printf("byte2 = %02x %s\n", byte2, bits2);
	
	return 0;
}
