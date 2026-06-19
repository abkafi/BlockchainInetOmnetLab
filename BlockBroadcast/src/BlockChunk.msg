import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

namespace inet;

class BlockChunk extends FieldsChunk
{
    int blockId;
    int prevHash;
    int hash;
    int txCount;
}