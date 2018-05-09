/*
 * MIT License, see root folder for full license.
 */
#include "banano.h"
#include "base-encoding.h"

/** current public key text. */
char current_public_key_text[MAX_PUBLIC_KEY_DIGITS];

/** current transaction address text. */
char current_tx_address_text[MAX_TX_ADDRESS_DIGITS];

/** the input to the current tx amount */
unsigned char current_tx_amount_char[TX_AMOUNT_BYTE_LENGTH+1];

/** */
static const char NO_KEY[] = "THE LEDGER ISWAITING FOR APUBLIC KEY TOBE  REQUESTEDFROM AN APP.";

/** */
static const char KEY_PREFIX_BAN[] = "BAN_";

static void update_public_key_display() {
	os_memset(current_public_key_display,'\0',sizeof(current_public_key_display));
	unsigned int c = 0;
	for(unsigned int y = 0; y < NUM_TEXT_DISPLAY_LINES; y++) {
		for(unsigned int x = 0; x < NUM_TEXT_DISPLAY_WIDTH-1; x++) {
			current_public_key_display[y][x] = current_public_key_text[c];
			c++;
		}
		current_public_key_display[y][NUM_TEXT_DISPLAY_WIDTH-1] = '\0';
	}
}

void update_no_public_key_data() {
	os_memset(current_public_key_text,'\0',sizeof(current_public_key_text));
	for(unsigned int c = 0; c < sizeof(NO_KEY); c++) {
		current_public_key_text[c] = NO_KEY[c];
	}

	update_public_key_display();

	C_icon_idle = C_icon_default;

	viewNeedsRefresh = 0;
}

void update_public_key_data(const ed25519_public_key * public_key, bagl_icon_details_t const C_icon) {
	os_memset(current_public_key_text,'\0',sizeof(current_public_key_text));
	unsigned int c = 0;
	for(; c < sizeof(KEY_PREFIX_BAN)-1; c++) {
		current_public_key_text[c] = KEY_PREFIX_BAN[c];
	}

	current_public_key_text[c] = '?';
	c++;

	encode_base_32((void *)public_key,sizeof(ed25519_public_key),current_public_key_text+c,sizeof(current_public_key_text)-c);

	update_public_key_display();

	C_icon_idle = C_icon;
}

/** display the screen with the tx address */
void update_tx_address_data(void) {
	for(unsigned int c = 0; c < sizeof(current_tx_address_text); c++) {
		current_tx_address_text[c] = '0';
	}
	for(unsigned int c = 0; c < sizeof(KEY_PREFIX_BAN); c++) {
		current_tx_address_text[c] = KEY_PREFIX_BAN[c];
	}

	current_tx_address_text[sizeof(KEY_PREFIX_BAN)] = '?';

	const unsigned int prefix_offset = sizeof(KEY_PREFIX_BAN) + 1;
	encode_base_32((void *)raw_tx,32,current_tx_address_text+prefix_offset,sizeof(current_tx_address_text)-prefix_offset);

	unsigned int c = 0;
	for(unsigned int x = 0; x < MAX_TX_ADDRESS_DIGITS; x++) {
		current_tx_address_display[x] = current_public_key_text[c];
		c++;
	}
	current_tx_address_display[MAX_TX_ADDRESS_DIGITS] = '\0';
}

unsigned int min_tx_len(void) {
	const unsigned int raw_tx_len_except_bip44_and_offset = raw_tx_len - (BIP44_BYTE_LENGTH + TX_AMOUNT_BYTE_OFFSET);
	if(raw_tx_len_except_bip44_and_offset < TX_AMOUNT_BYTE_LENGTH) {
		return raw_tx_len_except_bip44_and_offset;
	} else {
		return TX_AMOUNT_BYTE_LENGTH;
	}
}

/** display the screen with the tx amount */
void update_tx_amount_data(void) {
	for(unsigned int c = 0; c < sizeof(current_tx_amount_text); c++) {
		current_tx_amount_text[c] = ' ';
	}

	const unsigned char * raw_tx_amount_offset = raw_tx + TX_AMOUNT_BYTE_OFFSET;
	const unsigned int in_length = min_tx_len();
	for(unsigned int c = 0; c < TX_AMOUNT_BYTE_LENGTH; c++) {
		if(c < in_length) {
			current_tx_amount_char[c] = *raw_tx_amount_offset;
		} else {
			current_tx_amount_char[c] = '\0';
		}
	}
	current_tx_amount_char[TX_AMOUNT_BYTE_LENGTH] = '\0';

	const void * in = (void *)current_tx_amount_char;
	encode_base_10(in,in_length,current_tx_amount_text,sizeof(current_tx_amount_text));
}
