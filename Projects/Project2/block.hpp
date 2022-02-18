// #define BLOCK_LOW(id,p,n)  ((i)*(n)/(p))
// #define BLOCK_HIGH(id,p,n) \
//         (BLOCK_LOW((id)+1,p,n)-1)
// #define BLOCK_SIZE(id,p,n) \
//         (BLOCK_LOW((id)+1)-BLOCK_LOW(id))
// #define BLOCK_OWNER(index,p,n) \
//         (((p)*(index)+1)-1)/(n))

#define BLOCK_LOW(iRank, iSize, iN)  (3 + 2 * ((iRank)*(iN)/(iSize))) // Rank0 start from 3
#define BLOCK_HIGH(iRank, iSize, iN) (BLOCK_LOW((iRank)+1,(iSize),(iN))-2) // '-2' because we only check odd numbers
#define BLOCK_SIZE(iRank, iSize, iN) (BLOCK_HIGH((iRank), (iSize), (iN)) -  BLOCK_LOW((iRank), (iSize), (iN))) / 2 + 1
#define BLOCK_OWNER(index,iSize,iN) (((iSize)*(index)+1)-1)/(iN))