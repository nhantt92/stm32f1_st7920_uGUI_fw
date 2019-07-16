#ifndef INCLUDE_RINGBUF_H_
#define INCLUDE_RINGBUF_H_

typedef struct
{
	uint8_t *buf;
	uint32_t size;
	uint32_t pw;
	uint32_t pr;
} ringbuf;

void ringbuf_init(ringbuf *rb, uint8_t *buf, uint32_t size);
uint32_t ringbuf_get(ringbuf *rb, uint8_t *data);
uint32_t ringbuf_put(ringbuf *rb, uint8_t data);
#endif /* INCLUDE_RINGBUF_H_ */
