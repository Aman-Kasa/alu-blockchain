#include "blockchain.h"

/* Reads exactly n bytes from f into p; evaluates to 1 on failure */
#define RD(p, n) (fread((p), (n), 1, f) != 1)

/**
 * read_tx - deserializes one transaction
 * @f: open file stream
 * @swap: 1 if multi-byte fields must be byte-swapped
 *
 * Return: the allocated transaction, or NULL upon failure
 */
static transaction_t *read_tx(FILE *f, int swap)
{
	transaction_t *tx = calloc(1, sizeof(*tx));
	int32_t nb_in, nb_out, i;
	tx_in_t *in;
	tx_out_t *out;

	if (!tx)
		return (NULL);
	tx->inputs = llist_create(MT_SUPPORT_FALSE);
	tx->outputs = llist_create(MT_SUPPORT_FALSE);
	if (!tx->inputs || !tx->outputs || RD(tx->id, SHA256_DIGEST_LENGTH) ||
		RD(&nb_in, 4) || RD(&nb_out, 4))
		return (transaction_destroy(tx), NULL);
	if (swap)
		SWAPENDIAN(nb_in), SWAPENDIAN(nb_out);
	for (i = 0; i < nb_in; i++)
	{
		in = calloc(1, sizeof(*in));
		if (!in || RD(in->block_hash, SHA256_DIGEST_LENGTH) ||
			RD(in->tx_id, SHA256_DIGEST_LENGTH) ||
			RD(in->tx_out_hash, SHA256_DIGEST_LENGTH) ||
			RD(in->sig.sig, SIG_MAX_LEN) || RD(&in->sig.len, 1) ||
			llist_add_node(tx->inputs, in, ADD_NODE_REAR))
			return (free(in), transaction_destroy(tx), NULL);
	}
	for (i = 0; i < nb_out; i++)
	{
		out = calloc(1, sizeof(*out));
		if (!out || RD(&out->amount, 4) || RD(out->pub, EC_PUB_LEN) ||
			RD(out->hash, SHA256_DIGEST_LENGTH))
			return (free(out), transaction_destroy(tx), NULL);
		if (swap)
			SWAPENDIAN(out->amount);
		if (llist_add_node(tx->outputs, out, ADD_NODE_REAR))
			return (free(out), transaction_destroy(tx), NULL);
	}
	return (tx);
}

/**
 * read_block - deserializes one Block and its transactions
 * @f: open file stream
 * @swap: 1 if multi-byte fields must be byte-swapped
 *
 * Return: the allocated Block, or NULL upon failure
 */
static block_t *read_block(FILE *f, int swap)
{
	block_t *block = calloc(1, sizeof(*block));
	int32_t nb_tx, i;
	transaction_t *tx;

	if (!block)
		return (NULL);
	if (RD(&block->info, sizeof(block->info)) || RD(&block->data.len, 4))
		return (free(block), NULL);
	if (swap)
	{
		SWAPENDIAN(block->info.index), SWAPENDIAN(block->info.difficulty);
		SWAPENDIAN(block->info.timestamp), SWAPENDIAN(block->info.nonce);
		SWAPENDIAN(block->data.len);
	}
	if (block->data.len > BLOCKCHAIN_DATA_MAX ||
		(block->data.len && RD(block->data.buffer, block->data.len)) ||
		RD(block->hash, SHA256_DIGEST_LENGTH) || RD(&nb_tx, 4))
		return (free(block), NULL);
	if (swap)
		SWAPENDIAN(nb_tx);
	if (nb_tx == -1)
		return (block);
	block->transactions = llist_create(MT_SUPPORT_FALSE);
	if (!block->transactions)
		return (free(block), NULL);
	for (i = 0; i < nb_tx; i++)
	{
		tx = read_tx(f, swap);
		if (!tx || llist_add_node(block->transactions, tx, ADD_NODE_REAR))
			return (transaction_destroy(tx), block_destroy(block), NULL);
	}
	return (block);
}

/**
 * read_unspent - deserializes one unspent transaction output (165 bytes)
 * @f: open file stream
 * @swap: 1 if multi-byte fields must be byte-swapped
 *
 * Return: the allocated unspent transaction output, or NULL upon failure
 */
static unspent_tx_out_t *read_unspent(FILE *f, int swap)
{
	unspent_tx_out_t *utxo = calloc(1, sizeof(*utxo));

	if (!utxo)
		return (NULL);
	if (RD(utxo->block_hash, SHA256_DIGEST_LENGTH) ||
		RD(utxo->tx_id, SHA256_DIGEST_LENGTH) ||
		RD(&utxo->out.amount, 4) || RD(utxo->out.pub, EC_PUB_LEN) ||
		RD(utxo->out.hash, SHA256_DIGEST_LENGTH))
		return (free(utxo), NULL);
	if (swap)
		SWAPENDIAN(utxo->out.amount);
	return (utxo);
}

/**
 * read_lists - deserializes all the Blocks then all the unspent outputs
 * @f: open file stream, positioned right after the header
 * @bc: Blockchain whose chain and unspent lists are filled
 * @swap: 1 if multi-byte fields must be byte-swapped
 * @nb_blocks: number of Blocks to read
 * @nb_unspent: number of unspent transaction outputs to read
 *
 * Return: 0 upon success, or -1 upon failure
 */
static int read_lists(FILE *f, blockchain_t *bc, int swap,
	int32_t nb_blocks, int32_t nb_unspent)
{
	int32_t i;
	block_t *block;
	unspent_tx_out_t *utxo;

	for (i = 0; i < nb_blocks; i++)
	{
		block = read_block(f, swap);
		if (!block || llist_add_node(bc->chain, block, ADD_NODE_REAR))
			return (block_destroy(block), -1);
	}
	for (i = 0; i < nb_unspent; i++)
	{
		utxo = read_unspent(f, swap);
		if (!utxo || llist_add_node(bc->unspent, utxo, ADD_NODE_REAR))
			return (free(utxo), -1);
	}
	return (0);
}

/**
 * blockchain_deserialize - deserializes a Blockchain from a file
 * @path: path to the file to load the Blockchain from
 *
 * Description: fails if the file can't be opened/read, if the magic
 * number is wrong, or if the version doesn't match HBLK_VERSION
 *
 * Return: pointer to the deserialized Blockchain, or NULL upon failure
 */
blockchain_t *blockchain_deserialize(char const *path)
{
	FILE *f;
	uint8_t hdr[8];
	int32_t nb_blocks, nb_unspent;
	int swap;
	blockchain_t *bc;

	if (!path)
		return (NULL);
	f = fopen(path, "rb");
	if (!f)
		return (NULL);
	if (RD(hdr, 8) || memcmp(hdr, HBLK_MAGIC, 4) ||
		memcmp(hdr + 4, HBLK_VERSION, 3) || (hdr[7] != 1 && hdr[7] != 2) ||
		RD(&nb_blocks, 4) || RD(&nb_unspent, 4))
		return (fclose(f), NULL);
	swap = hdr[7] != _get_endianness();
	if (swap)
		SWAPENDIAN(nb_blocks), SWAPENDIAN(nb_unspent);
	bc = calloc(1, sizeof(*bc));
	if (!bc)
		return (fclose(f), NULL);
	bc->chain = llist_create(MT_SUPPORT_TRUE);
	bc->unspent = llist_create(MT_SUPPORT_TRUE);
	if (!bc->chain || !bc->unspent ||
		read_lists(f, bc, swap, nb_blocks, nb_unspent))
		return (fclose(f), blockchain_destroy(bc), NULL);
	fclose(f);
	return (bc);
}
