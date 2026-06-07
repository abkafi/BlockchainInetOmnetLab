import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

namespace inet;

class BlockChunk extends FieldsChunk
{
    int blockId = 0;

    int previousBlockHash = 0;

    int blockHash = 0;
}