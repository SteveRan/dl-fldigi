
#include <stdint.h>

#ifndef INC_SSDV_H
#define INC_SSDV_H
#ifdef __cplusplus
extern "C" {
#endif

#define SSDV_ERROR       (-1)
#define SSDV_OK          (0)
#define SSDV_FEED_ME     (1)
#define SSDV_HAVE_PACKET (2)
#define SSDV_BUFFER_FULL (3)
#define SSDV_EOI         (4)

/* Packet details */
#define SSDV_PKT_SIZE         (0x100)
#define SSDV_PKT_SIZE_HEADER  (0x0B)
#define SSDV_PKT_SIZE_RSCODES (0x20)
#define SSDV_PKT_SIZE_CRC     (0x02)
#define SSDV_PKT_SIZE_PAYLOAD (SSDV_PKT_SIZE - SSDV_PKT_SIZE_HEADER - SSDV_PKT_SIZE_CRC - SSDV_PKT_SIZE_RSCODES)

#define HBUFF_LEN (16)
#define COMPONENTS (3)

typedef struct
{
	/* Image information */
	uint16_t width;
	uint16_t height;
	uint8_t image_id;
	uint16_t packet_id;
	uint16_t mcu_id;
	uint16_t mcu_count;
	uint16_t packet_mcu_id;
	uint16_t packet_mcu_offset;
	
	/* Source buffer */
	uint8_t *inp;      /* Pointer to next input byte                    */
	size_t in_len;     /* Number of input bytes remaining               */
	size_t in_skip;    /* Number of input bytes to skip                 */
	
	/* Source bits */
	uint32_t workbits; /* Input bits currently being worked on          */
	uint8_t worklen;   /* Number of bits in the input bit buffer        */
	
	/* JPEG / Packet output buffer */
	uint8_t *out;      /* Pointer to the beginning of the output buffer */
	uint8_t *outp;     /* Pointer to the next output byte               */
	size_t out_len;    /* Number of output bytes remaining              */
	char out_stuff;    /* Flag to add stuffing bytes to output          */
	
	/* Output bits */
	uint32_t outbits;  /* Output bit buffer                             */
	uint8_t outlen;    /* Number of bits in the output bit buffer       */
	
	/* JPEG decoder state */
	enum {
		S_MARKER = 0,
		S_MARKER_LEN,
		S_MARKER_DATA,
		S_HUFF,
		S_INT,
		S_EOI,
	} state;
	uint16_t marker;    /* Current marker                               */
	uint16_t marker_len; /* Length of data following marker             */
	uint8_t *marker_data; /* Where to copy marker data too              */
	uint16_t marker_data_len; /* How much is there                      */
	uint8_t component;  /* 0 = Y, 1 = Cb, 2 = Cr                        */
	uint8_t mcupart;    /* 0-3 = Y, 4 = Cb, 5 = Cr                      */
	uint8_t acpart;     /* 0 - 64; 0 = DC, 1 - 64 = AC                  */
	int dc[COMPONENTS]; /* DC value for each component                  */
	uint8_t acrle;      /* RLE value for current AC value               */
	char dcmode;        /* 0 = Absolute, 1 = Relative (parts 0, 4 & 5)  */
	char needbits;      /* Number of bits needed to decode integer      */
	
	/* Small buffer for reading SOF0 and SOS header data into */
	uint8_t hbuff[HBUFF_LEN];
	
} ssdv_t;

typedef struct {
	uint8_t  image_id;
	uint16_t packet_id;
	uint16_t width;
	uint16_t height;
	uint16_t mcu_offset;
	uint16_t mcu_id;
	uint16_t mcu_count;
} ssdv_packet_info_t;

/* Encoding */
extern char ssdv_enc_init(ssdv_t *s, uint8_t image_id);
extern char ssdv_enc_set_buffer(ssdv_t *s, uint8_t *buffer);
extern char ssdv_enc_get_packet(ssdv_t *s);
extern char ssdv_enc_feed(ssdv_t *s, uint8_t *buffer, size_t length);

/* Decoding */
extern char ssdv_dec_init(ssdv_t *s);
extern char ssdv_dec_feed(ssdv_t *s, uint8_t *packet);
extern char ssdv_dec_get_jpeg(ssdv_t *s, uint8_t **jpeg, size_t *length);

extern char ssdv_dec_is_packet(uint8_t *packet, int *errors);
extern void ssdv_dec_header(ssdv_packet_info_t *info, uint8_t *packet);

#ifdef __cplusplus
}
#endif
#endif

