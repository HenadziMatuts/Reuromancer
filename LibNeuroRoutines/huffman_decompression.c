#include "neuro_routines.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

typedef struct node_t {
	uint8_t value;
	struct node_t *left, *right;
} node_t;

static uint8_t *g_src = NULL;
static unsigned char g_mask = 0;
static int g_byte = 0;

static int getbits(int numbits)
{
	int i, bits = 0;

	for (i = 0; i < numbits; i++)
	{
		if (g_mask == 0) {
			g_byte = *g_src++;
			g_mask = 0x80;
		}
		bits <<= 1;
		bits |= g_byte & g_mask ? 1 : 0;
		g_mask >>= 1;
	}

	return bits;
}

static node_t* build_tree(void)
{
	node_t *node = (node_t*)calloc(1, sizeof(node_t));
	assert(node);

	if (getbits(1)) {
		node->right = NULL;
		node->left = NULL;
		node->value = getbits(8);
	}
	else {
		node->right = build_tree();
		node->left = build_tree();
		node->value = 0;
	}

	return node;
}

static uint32_t fgetl_le()
{
	uint8_t c1, c2, c3, c4;

	c1 = *g_src++;
	c2 = *g_src++;
	c3 = *g_src++;
	c4 = *g_src++;

	return (c4 << 24) + (c3 << 16) + (c2 << 8) + c1;
}

void destroy_tree(node_t **root)
{
	node_t *node = *root;

	if (node->left) {
		destroy_tree(&node->right);
		destroy_tree(&node->left);
	}

	free(node);
	node = NULL;
}

int huffman_decompress(uint8_t *src, uint8_t *dst)
{
	int length, i = 0;
	node_t *root, *node;

	g_src = src;

	length = fgetl_le();
	node = root = build_tree();

	while (i < length)
	{
		node = getbits(1) ? node->left : node->right;
		if (!node->left) {
			dst[i++] = node->value;
			node = root;
		}
	}

	destroy_tree(&root);

	g_src = NULL;
	g_mask = 0;
	g_byte = 0;

	return length;
}
