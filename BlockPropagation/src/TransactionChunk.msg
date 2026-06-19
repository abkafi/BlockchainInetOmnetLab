import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

namespace inet;

class TransactionChunk extends FieldsChunk
{
    int txId = 0;
    string sender;
    string receiver;
    double amount = 0;
}