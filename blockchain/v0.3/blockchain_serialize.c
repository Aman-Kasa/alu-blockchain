#include "blockchain.h"

/**
 * write_tx - serializes one transaction
 * @tx: transaction to serialize
 * @f: open file stream
 *
 * Description: id (32), nb_inputs (4), nb_outputs (4), then each input
 * on 169 bytes and each output on 101 bytes
 */
static void write_tx(transaction_t const *tx, FILE *f)
{
	int32_t nb_in = llist_size(tx->inputs), nb_out = llist_size(tx->outputs);
	int32_t i;
	tx_in_t *in;
	tx_out_t *out;

	nb_in = nb_in < 0 ? 0 : nb_in;
	nb_out = nb_out < 0 ? 0 : nb_out;
	fwrite(tx->id, SHA256_DIGEST_LENGTH, 1, f);
	fwrite(&nb_in, 4, 1, f);
	fwrite(&nb_out, 4, 1, f);
	for (i = 0; i < nb_in; i++)
	{
		in = llist_get_node_at(tx->inputs, i);
		fwrite(in->block_hash, SHA256_DIGEST_LENGTH, 1, f);
		fwrite(in->tx_id, SHA256_DIGEST_LENGTH, 1, f);
		fwrite(in->tx_out_hash, SHA256_DIGEST_LENGTH, 1, f);
		fwrite(in->sig.sig, SIG_MAX_LEN, 1, f);
		fwrite(&in->sig.len, 1, 1, f);
	}
	for (i = 0; i < nb_out; i++)
	{
		out = llist_get_node_at(tx->outputs, i);
		fwrite(&out->amount, 4, 1, f);
		fwrite(out->pub, EC_PUB_LEN, 1, f);
		fwrite(out->hash, SHA256_DIGEST_LENGTH, 1, f);
	}
}

/**
 * write_block - serializes one Block and its transactions
 * @block: Block to serialize
 * @f: open file stream
 *
 * Description: nb_transactions is -1 when the list is NULL (Genesis)
 */
static void write_block(block_t const *block, FILE *f)
{
	int32_t nb_tx, i;

	fwrite(&block->info, sizeof(block->info), 1, f);
	fwrite(&block->data.len, 4, 1, f);
	fwrite(block->data.buffer, block->data.len, 1, f);
	fwrite(block->hash, SHA256_DIGEST_LENGTH, 1, f);
	nb_tx = block->transactions ? llist_size(block->transactions) : -1;
	fwrite(&nb_tx, 4, 1, f);
	for (i = 0; i < nb_tx; i++)
		write_tx(llist_get_node_at(block->transactions, i), f);
}

/**
 * write_unspent - serializes one unspent transaction output (165 bytes)
 * @utxo: unspent transaction output to serialize
 * @f: open file stream
 */
static void write_unspent(unspent_tx_out_t const *utxo, FILE *f)
{
	fwrite(utxo->block_hash, SHA256_DIGEST_LENGTH, 1, f);
	fwrite(utxo->tx_id, SHA256_DIGEST_LENGTH, 1, f);
	fwrite(&utxo->out.amount, 4, 1, f);
	fwrite(utxo->out.pub, EC_PUB_LEN, 1, f);
	fwrite(utxo->out.hash, SHA256_DIGEST_LENGTH, 1, f);
}

/**
 * blockchain_serialize - serializes a Blockchain into a file
 * @blockchain: pointer to the Blockchain to be serialized
 * @path: path to the file to write (overwritten if it exists)
 *
 * Return: 0 upon success, or -1 upon failure
 */
int blockchain_serialize(blockchain_t const *blockchain, char const *path)
{
	FILE *f;
	int32_t nb_blocks, nb_unspent, i;
	uint8_t endian = _get_endianness();
	int err;

	if (!blockchain || !blockchain->chain || !path)
		return (-1);
	nb_blocks = llist_size(blockchain->chain);
	nb_unspent = blockchain->unspent ? llist_size(blockchain->unspent) : 0;
	if (nb_blocks < 0 || nb_unspent < 0)
		return (-1);
	f = fopen(path, "wb");
	if (!f)
		return (-1);
	fwrite(HBLK_MAGIC, 4, 1, f);
	fwrite(HBLK_VERSION, 3, 1, f);
	fwrite(&endian, 1, 1, f);
	fwrite(&nb_blocks, 4, 1, f);
	fwrite(&nb_unspent, 4, 1, f);
	for (i = 0; i < nb_blocks; i++)
		write_block(llist_get_node_at(blockchain->chain, i), f);
	for (i = 0; i < nb_unspent; i++)
		write_unspent(llist_get_node_at(blockchain->unspent, i), f);
	err = ferror(f);
	if (fclose(f) || err)
		return (-1);
	return (0);
}
