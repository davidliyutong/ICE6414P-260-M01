#define BLOCK_LOW(iRank,iSize,iN)  ((iRank) * (iN) / (iSize))
#define BLOCK_HIGH(iRank,iSize,iN) (BLOCK_LOW(((iRank) + 1), iSize, iN) - 1)
#define BLOCK_SIZE(iRank,iSize,iN) (BLOCK_LOW((iRank) + 1, iSize, iN) - BLOCK_LOW((iRank), iSize, iN))
#define BLOCK_OWNER(iIndex,iSize,iN) (((iSize) * (iIndex)+1) - 1) / (iN))