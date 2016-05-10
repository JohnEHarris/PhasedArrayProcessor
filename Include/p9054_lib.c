////////////////////////////////////////////////////////////////
// File - P9054_LIB.C
//
// Library for 'DriverBuilder for PLX 9054' API. 
// The basic idea is to get a handle for the board
// with P9054_Open() and use it in the rest of the program
// when calling WD functions.  Call P9054_Close() when done.
// 
////////////////////////////////////////////////////////////////

#include "p9054_lib.h"
/* rah 
#include "../../../include/windrvr_int_thread.h"
*/

/* rah - localized the includes */
#include "../include/windrvr_int_thread.h"

#if !defined(__KERNEL__) 
	#include <stdio.h>
#endif

// this string is set to an error message, if one occurs
CHAR P9054_ErrorString[1024];

// internal data structures and enums
enum { P9054_DMA_CHANNEL_SHIFT = 0x14 }; // shift in address between channels 0 and 1 of DMA

typedef struct P9054_DMA_STRUCT{
    WD_DMA dma;
    WD_DMA dmaList;
    P9054_DMA_CHANNEL dmaChannel;
} P9054_DMA_STRUCT;

enum { P9054_MODE_DESC       = 0xF9000140 };
enum { P9054_MODE_DESC_BYTE  = 0x00000000 };
enum { P9054_MODE_DESC_WORD  = 0x00010001 };
enum { P9054_MODE_DESC_DWORD = 0x00030003 };

typedef struct
{
    WD_INTERRUPT Int;
    HANDLE hThread;
    WD_TRANSFER Trans[2];
    P9054_INT_HANDLER funcIntHandler;
} P9054_INTERRUPT;

typedef struct 
{
    DWORD dwLocalBase;
    DWORD dwMask;
    DWORD dwBytes;
    DWORD dwAddr;
    DWORD dwAddrDirect;
    BOOL  fIsMemory;
} P9054_ADDR_DESC;

typedef struct P9054_STRUCT
{
    HANDLE hWD;
    WD_CARD cardLock;
    WD_PCI_SLOT pciSlot;
    WD_CARD_REGISTER cardReg;
    P9054_ADDR_DESC addrDesc[AD_PCI_BARS];
    DWORD  addrSpace;
    BOOL   fUseInt;
    P9054_INTERRUPT Int;
} P9054_STRUCT;


// internal function used by P9054_Open()
BOOL P9054_DetectCardElements(P9054_HANDLE hPlx);
// internal function used by P9054_Read... and P9054_Write... functions
void P9054_SetMode (P9054_HANDLE hPlx, DWORD dwLocalAddr);

DWORD P9054_CountCards (DWORD dwVendorID, DWORD dwDeviceID)
{
    WD_VERSION ver;
    WD_PCI_SCAN_CARDS pciScan;
    HANDLE hWD = INVALID_HANDLE_VALUE;

    P9054_ErrorString[0] = '\0';
    hWD = WD_Open();
    // check if handle valid & version OK
    if (hWD==INVALID_HANDLE_VALUE) 
    {
        sprintf( P9054_ErrorString, "Failed opening " WD_PROD_NAME " device\n");
        return 0;
    }

    BZERO(ver);
    WD_Version(hWD,&ver);
    if (ver.dwVer<WD_VER) 
    {
        sprintf( P9054_ErrorString, "Incorrect " WD_PROD_NAME " version\n");
        WD_Close (hWD);
        return 0;
    }

    BZERO(pciScan);
    pciScan.searchId.dwVendorId = dwVendorID;
    pciScan.searchId.dwDeviceId = dwDeviceID;
    WD_PciScanCards (hWD, &pciScan);
    WD_Close (hWD);
    if (pciScan.dwCards==0)
        sprintf( P9054_ErrorString, "no cards found\n");
    return pciScan.dwCards;
}


BOOL P9054_Open (P9054_HANDLE *phPlx, DWORD dwVendorID, DWORD dwDeviceID, DWORD nCardNum, DWORD dwOptions)
{
    P9054_HANDLE hPlx = (P9054_HANDLE) malloc (sizeof (P9054_STRUCT));
    DWORD dwIntStatus;

    WD_VERSION ver;
    WD_PCI_SCAN_CARDS pciScan;
    WD_PCI_CARD_INFO pciCardInfo;

    *phPlx = NULL;
    P9054_ErrorString[0] = '\0';
    BZERO(*hPlx);

    hPlx->hWD = WD_Open();

    // check if handle valid & version OK
    if (hPlx->hWD==INVALID_HANDLE_VALUE) 
    {
        sprintf( P9054_ErrorString, "Failed opening " WD_PROD_NAME " device\n");
        goto Exit;
    }

    BZERO(ver);
    WD_Version(hPlx->hWD,&ver);
    if (ver.dwVer<WD_VER) 
    {
        sprintf( P9054_ErrorString, "Incorrect " WD_PROD_NAME " version\n");
        goto Exit;
    }

    BZERO(pciScan);
    pciScan.searchId.dwVendorId = dwVendorID;
    pciScan.searchId.dwDeviceId = dwDeviceID;
    WD_PciScanCards (hPlx->hWD, &pciScan);
    if (pciScan.dwCards==0) // Found at least one card
    {
        sprintf( P9054_ErrorString, "Could not find PCI card\n");
        goto Exit;
    }
    if (pciScan.dwCards<=nCardNum)
    {
        sprintf( P9054_ErrorString, "Card out of range of available cards\n");
        goto Exit;
    }

    BZERO(pciCardInfo);
    pciCardInfo.pciSlot = pciScan.cardSlot[nCardNum];
    WD_PciGetCardInfo (hPlx->hWD, &pciCardInfo);
    hPlx->pciSlot = pciCardInfo.pciSlot;
    hPlx->cardReg.Card = pciCardInfo.Card;

    hPlx->fUseInt = (dwOptions & P9054_OPEN_USE_INT) ? TRUE : FALSE;
    if (!hPlx->fUseInt)
    {
        DWORD i;
        // Remove interrupt item if not needed
        for (i=0; i<hPlx->cardReg.Card.dwItems; i++)
        {
            WD_ITEMS *pItem = &hPlx->cardReg.Card.Item[i];
            if (pItem->item==ITEM_INTERRUPT)
                pItem->item = ITEM_NONE;
        }
    }
    else
    {
        DWORD i;
        // make interrupt resource sharable
        for (i=0; i<hPlx->cardReg.Card.dwItems; i++)
        {
            WD_ITEMS *pItem = &hPlx->cardReg.Card.Item[i];
            if (pItem->item==ITEM_INTERRUPT)
                pItem->fNotSharable = FALSE;
        }
    }

    hPlx->cardReg.fCheckLockOnly = FALSE;
    WD_CardRegister (hPlx->hWD, &hPlx->cardReg);
    if (hPlx->cardReg.hCard==0)
    {
        sprintf ( P9054_ErrorString, "Failed locking device\n");
        goto Exit;
    }

    if (!P9054_DetectCardElements(hPlx))
    {
        sprintf ( P9054_ErrorString, "Card does not have all items expected for PLX 9054\n");
        goto Exit;
    }

    // this enables target abort so it wont get stuck 
    dwIntStatus = P9054_ReadReg (hPlx, P9054_INTCSR);
    P9054_WriteReg (hPlx, P9054_INTCSR, dwIntStatus | BIT12);

    // Open finished OK
    *phPlx = hPlx;
    return TRUE;

Exit:
    // Error durin Open
    if (hPlx->cardReg.hCard) 
        WD_CardUnregister(hPlx->hWD, &hPlx->cardReg);
    if (hPlx->hWD!=INVALID_HANDLE_VALUE)
        WD_Close(hPlx->hWD);
    free (hPlx);
    return FALSE;
}

void P9054_GetPciSlot(P9054_HANDLE hPlx, WD_PCI_SLOT *pPciSlot)
{
    *pPciSlot = hPlx->pciSlot;
}

DWORD P9054_ReadPCIReg(P9054_HANDLE hPlx, DWORD dwReg)
{
    WD_PCI_CONFIG_DUMP pciCnf;
    DWORD dwVal;

    BZERO (pciCnf);
    pciCnf.pciSlot = hPlx->pciSlot;
    pciCnf.pBuffer = &dwVal;
    pciCnf.dwOffset = dwReg;
    pciCnf.dwBytes = 4;
    pciCnf.fIsRead = TRUE;
    WD_PciConfigDump(hPlx->hWD,&pciCnf);
    return dwVal;
}

void P9054_WritePCIReg(P9054_HANDLE hPlx, DWORD dwReg, DWORD dwData)
{
    WD_PCI_CONFIG_DUMP pciCnf;

    BZERO (pciCnf);
    pciCnf.pciSlot = hPlx->pciSlot;
    pciCnf.pBuffer = &dwData;
    pciCnf.dwOffset = dwReg;
    pciCnf.dwBytes = 4;
    pciCnf.fIsRead = FALSE;
    WD_PciConfigDump(hPlx->hWD,&pciCnf);
}

BOOL P9054_DetectCardElements(P9054_HANDLE hPlx)
{
    DWORD i;
    DWORD ad_sp;

    BZERO(hPlx->Int);
    BZERO(hPlx->addrDesc);

    for (i=0; i<hPlx->cardReg.Card.dwItems; i++)
    {
        WD_ITEMS *pItem = &hPlx->cardReg.Card.Item[i];

        switch (pItem->item)
        {
        case ITEM_MEMORY:
        case ITEM_IO:
            {
                DWORD dwBytes;
                DWORD dwAddr;
                DWORD dwPhysAddr;
                DWORD dwAddrDirect = 0;
                BOOL fIsMemory;
                if (pItem->item==ITEM_MEMORY)
                {
                    dwBytes = pItem->I.Mem.dwBytes;
                    dwAddr = pItem->I.Mem.dwTransAddr;
                    dwAddrDirect = pItem->I.Mem.dwUserDirectAddr;
                    dwPhysAddr = pItem->I.Mem.dwPhysicalAddr;
                    fIsMemory = TRUE;
                }
                else 
                {
                    dwBytes = pItem->I.IO.dwBytes;
                    dwAddr = pItem->I.IO.dwAddr;
                    dwPhysAddr = dwAddr & 0xffff;
                    fIsMemory = FALSE;
                }

                for (ad_sp=P9054_ADDR_REG; ad_sp<=P9054_ADDR_EPROM; ad_sp++)
                {
                    DWORD dwPCIAddr;
                    DWORD dwPCIReg;

                    if (hPlx->addrDesc[ad_sp].dwAddr) continue;
                    if (ad_sp==P9054_ADDR_REG) dwPCIReg = PCI_BAR0;
                    else if (ad_sp<P9054_ADDR_EPROM) 
                        dwPCIReg = PCI_BAR2 + 4*(ad_sp-P9054_ADDR_SPACE0);
                    else dwPCIReg = PCI_ERBAR;
                    dwPCIAddr = P9054_ReadPCIReg(hPlx, dwPCIReg);
                    if (dwPCIAddr & 1)
                    {
                        if (fIsMemory) continue;
                        dwPCIAddr &= ~0x3;
                    }
                    else
                    {
                        if (!fIsMemory) continue;
                        dwPCIAddr &= ~0xf;
                    }
                    if (dwPCIAddr==dwPhysAddr)
                        break;
                }
                if (ad_sp<=P9054_ADDR_EPROM)
                {
                    DWORD j;
                    hPlx->addrDesc[ad_sp].dwBytes = dwBytes;
                    hPlx->addrDesc[ad_sp].dwAddr = dwAddr;
                    hPlx->addrDesc[ad_sp].dwAddrDirect = dwAddrDirect;
                    hPlx->addrDesc[ad_sp].fIsMemory = fIsMemory;
                    hPlx->addrDesc[ad_sp].dwMask = 0;
                    for (j=1; j<hPlx->addrDesc[ad_sp].dwBytes && j!=0x80000000; j *= 2)
                    {
                        hPlx->addrDesc[ad_sp].dwMask = 
                            (hPlx->addrDesc[ad_sp].dwMask << 1) | 1;
                    }
                }
            }
            break;
        case ITEM_INTERRUPT:
            if (hPlx->Int.Int.hInterrupt) return FALSE;
            hPlx->Int.Int.hInterrupt = pItem->I.Int.hInterrupt;
            break;
        }
    }

    // check that all the items needed were found
    // check if interrupt found
    if (hPlx->fUseInt && !hPlx->Int.Int.hInterrupt) 
    {
        return FALSE;
    }

    // check that the registers space was found
    if (!P9054_IsAddrSpaceActive(hPlx, P9054_ADDR_REG)) 
            //|| hPlx->addrDesc[P9054_ADDR_REG].dwBytes!=P9054_RANGE_REG)
        return FALSE;

	// use address space 0 for accessing local addresses
    hPlx->addrSpace = P9054_ADDR_SPACE0;
    
    // check that address space 0 was found
    if (!P9054_IsAddrSpaceActive(hPlx, hPlx->addrSpace)) return FALSE;

    return TRUE;
}

void P9054_Close(P9054_HANDLE hPlx)
{
    // disable interrupts
    if (P9054_IntIsEnabled(hPlx))
        P9054_IntDisable(hPlx);

    // unregister card
    if (hPlx->cardReg.hCard) 
        WD_CardUnregister(hPlx->hWD, &hPlx->cardReg);

    // close DriverBuilder
    WD_Close(hPlx->hWD);

    free (hPlx);
}

BOOL P9054_IsAddrSpaceActive(P9054_HANDLE hPlx, P9054_ADDR addrSpace)
{
    return hPlx->addrDesc[addrSpace].dwAddr!=0;
}

DWORD P9054_ReadReg (P9054_HANDLE hPlx, DWORD dwReg)
{
    return P9054_ReadDWord(hPlx, P9054_ADDR_REG, dwReg);
}

void P9054_WriteReg (P9054_HANDLE hPlx, DWORD dwReg, DWORD dwData)
{
    P9054_WriteDWord(hPlx, P9054_ADDR_REG, dwReg, dwData);
}

BYTE P9054_ReadByte (P9054_HANDLE hPlx, P9054_ADDR addrSpace, DWORD dwOffset)
{
    if (hPlx->addrDesc[addrSpace].fIsMemory)
    {
        DWORD dwAddr = hPlx->addrDesc[addrSpace].dwAddrDirect + dwOffset;
        BYTE *pByte = (BYTE *) dwAddr;
        return *pByte;
    }
    else
    {
        DWORD dwAddr = hPlx->addrDesc[addrSpace].dwAddr + dwOffset;
        WD_TRANSFER trans;
        BZERO(trans);
        trans.cmdTrans = RP_BYTE;
        trans.dwPort = dwAddr;
        WD_Transfer (hPlx->hWD, &trans);
        return trans.Data.Byte;
    }
}

void P9054_WriteByte (P9054_HANDLE hPlx, P9054_ADDR addrSpace, DWORD dwOffset,
	BYTE data)
{
    if (hPlx->addrDesc[addrSpace].fIsMemory)
    {
        DWORD dwAddr = hPlx->addrDesc[addrSpace].dwAddrDirect + dwOffset;
        BYTE *pByte = (BYTE *) dwAddr;
        *pByte = data;
    }
    else
    {
        DWORD dwAddr = hPlx->addrDesc[addrSpace].dwAddr + dwOffset;
        WD_TRANSFER trans;
        BZERO(trans);
        trans.cmdTrans = WP_BYTE;
        trans.dwPort = dwAddr;
        trans.Data.Byte = data;
        WD_Transfer (hPlx->hWD, &trans);
    }
}

WORD P9054_ReadWord (P9054_HANDLE hPlx, P9054_ADDR addrSpace, DWORD dwOffset)
{
    if (hPlx->addrDesc[addrSpace].fIsMemory)
    {
        DWORD dwAddr = hPlx->addrDesc[addrSpace].dwAddrDirect + dwOffset;
        WORD *pWord = (WORD *) dwAddr;
        return *pWord;
    }
    else
    {
        DWORD dwAddr = hPlx->addrDesc[addrSpace].dwAddr + dwOffset;
        WD_TRANSFER trans;
        BZERO(trans);
        trans.cmdTrans = RP_WORD;
        trans.dwPort = dwAddr;
        WD_Transfer (hPlx->hWD, &trans);
        return trans.Data.Word;
    }
}

void P9054_WriteWord (P9054_HANDLE hPlx, P9054_ADDR addrSpace, DWORD dwOffset, WORD data)
{
    if (hPlx->addrDesc[addrSpace].fIsMemory)
    {
        DWORD dwAddr = hPlx->addrDesc[addrSpace].dwAddrDirect + dwOffset;
        WORD *pWord = (WORD *) dwAddr;
        *pWord = data;
    }
    else
    {
        DWORD dwAddr = hPlx->addrDesc[addrSpace].dwAddr + dwOffset;
        WD_TRANSFER trans;
        BZERO(trans);
        trans.cmdTrans = WP_WORD;
        trans.dwPort = dwAddr;
        trans.Data.Word = data;
        WD_Transfer (hPlx->hWD, &trans);
    }
}

DWORD P9054_ReadDWord (P9054_HANDLE hPlx, P9054_ADDR addrSpace, DWORD dwOffset)
{
    if (hPlx->addrDesc[addrSpace].fIsMemory)
    {
        DWORD dwAddr = hPlx->addrDesc[addrSpace].dwAddrDirect + dwOffset;
        DWORD *pDword = (DWORD *) dwAddr;
        return *pDword;
    }
    else
    {
        DWORD dwAddr = hPlx->addrDesc[addrSpace].dwAddr + dwOffset;
        WD_TRANSFER trans;
        BZERO(trans);
        trans.cmdTrans = RP_DWORD;
        trans.dwPort = dwAddr;
        WD_Transfer (hPlx->hWD, &trans);
        return trans.Data.Dword;
    }
}

void P9054_WriteDWord (P9054_HANDLE hPlx, P9054_ADDR addrSpace, DWORD dwOffset, DWORD data)
{
    if (hPlx->addrDesc[addrSpace].fIsMemory)
    {
        DWORD dwAddr = hPlx->addrDesc[addrSpace].dwAddrDirect + dwOffset;
        DWORD *pDword = (DWORD *) dwAddr;
        *pDword = data;
    }
    else
    {
        DWORD dwAddr = hPlx->addrDesc[addrSpace].dwAddr + dwOffset;
        WD_TRANSFER trans;
        BZERO(trans);
        trans.cmdTrans = WP_DWORD;
        trans.dwPort = dwAddr;
        trans.Data.Dword = data;
        WD_Transfer (hPlx->hWD, &trans);
    }
}

void P9054_ReadWriteBlock (P9054_HANDLE hPlx, DWORD dwOffset, PVOID buf, 
                    DWORD dwBytes, BOOL fIsRead, P9054_ADDR addrSpace, P9054_MODE mode)
{
    DWORD dwAddr = hPlx->addrDesc[addrSpace].dwAddr + dwOffset;
    WD_TRANSFER trans;

    BZERO(trans);

    if (hPlx->addrDesc[addrSpace].fIsMemory) 
    {
        if (fIsRead) 
        {
            if (mode==P9054_MODE_BYTE) trans.cmdTrans = RM_SBYTE;
            else if (mode==P9054_MODE_WORD) trans.cmdTrans = RM_SWORD;
            else trans.cmdTrans = RM_SDWORD;
        }
        else 
        {
            if (mode==P9054_MODE_BYTE) trans.cmdTrans = WM_SBYTE;
            else if (mode==P9054_MODE_WORD) trans.cmdTrans = WM_SWORD;
            else trans.cmdTrans = WM_SDWORD;
        }
    }
    else 
    {
        if (fIsRead) 
        {
            if (mode==P9054_MODE_BYTE) trans.cmdTrans = RP_SBYTE;
            else if (mode==P9054_MODE_WORD) trans.cmdTrans = RP_SWORD;
            else trans.cmdTrans = RP_SDWORD;
        }
        else 
        {
            if (mode==P9054_MODE_BYTE) trans.cmdTrans = WP_SBYTE;
            else if (mode==P9054_MODE_WORD) trans.cmdTrans = WP_SWORD;
            else trans.cmdTrans = WP_SDWORD;
        }
    }
    trans.dwPort = dwAddr;
    trans.fAutoinc = TRUE;
    trans.dwBytes = dwBytes;
    trans.dwOptions = 0;
    trans.Data.pBuffer = buf;
    WD_Transfer (hPlx->hWD, &trans);
}

void P9054_ReadBlock (P9054_HANDLE hPlx, DWORD dwOffset, PVOID buf, 
                    DWORD dwBytes, P9054_ADDR addrSpace, P9054_MODE mode)
{
    P9054_ReadWriteBlock (hPlx, dwOffset, buf, dwBytes, TRUE, addrSpace, mode);
}

void P9054_WriteBlock (P9054_HANDLE hPlx, DWORD dwOffset, PVOID buf, 
                     DWORD dwBytes, P9054_ADDR addrSpace, P9054_MODE mode)
{
    P9054_ReadWriteBlock (hPlx, dwOffset, buf, dwBytes, FALSE, addrSpace, mode);
}

void P9054_SetMode (P9054_HANDLE hPlx, DWORD dwLocalAddr)
{
    hPlx->addrDesc[hPlx->addrSpace].dwLocalBase = dwLocalAddr & ~hPlx->addrDesc[hPlx->addrSpace].dwMask;
    hPlx->addrDesc[hPlx->addrSpace].dwLocalBase |= BIT0;
    P9054_WriteReg (hPlx, P9054_LAS0BA, hPlx->addrDesc[hPlx->addrSpace].dwLocalBase);
}

BYTE P9054_ReadByteLocal (P9054_HANDLE hPlx, DWORD dwLocalAddr)
{
    DWORD dwOffset = hPlx->addrDesc[hPlx->addrSpace].dwMask & dwLocalAddr;
    P9054_SetMode (hPlx, dwLocalAddr);
    return P9054_ReadByte(hPlx, hPlx->addrSpace, dwOffset);
}

void P9054_WriteByteLocal (P9054_HANDLE hPlx, DWORD dwLocalAddr, BYTE data)
{
    DWORD dwOffset = hPlx->addrDesc[hPlx->addrSpace].dwMask & dwLocalAddr;
    P9054_SetMode (hPlx, dwLocalAddr);
    P9054_WriteByte(hPlx, hPlx->addrSpace, dwOffset, data);
}

WORD P9054_ReadWordLocal (P9054_HANDLE hPlx, DWORD dwLocalAddr)
{
    DWORD dwOffset = hPlx->addrDesc[hPlx->addrSpace].dwMask & dwLocalAddr;
    P9054_SetMode (hPlx, dwLocalAddr);
    return P9054_ReadWord(hPlx, hPlx->addrSpace, dwOffset);
}

void P9054_WriteWordLocal (P9054_HANDLE hPlx, DWORD dwLocalAddr, WORD data)
{
    DWORD dwOffset = hPlx->addrDesc[hPlx->addrSpace].dwMask & dwLocalAddr;
    P9054_SetMode (hPlx, dwLocalAddr);
    P9054_WriteWord(hPlx, hPlx->addrSpace, dwOffset, data);
}

DWORD P9054_ReadDWordLocal (P9054_HANDLE hPlx, DWORD dwLocalAddr)
{
    DWORD dwOffset = hPlx->addrDesc[hPlx->addrSpace].dwMask & dwLocalAddr;
    P9054_SetMode (hPlx, dwLocalAddr);
    return P9054_ReadDWord(hPlx, hPlx->addrSpace, dwOffset);
}

void P9054_WriteDWordLocal (P9054_HANDLE hPlx, DWORD dwLocalAddr, DWORD data)
{
    DWORD dwOffset = hPlx->addrDesc[hPlx->addrSpace].dwMask & dwLocalAddr;
    P9054_SetMode (hPlx, dwLocalAddr);
    P9054_WriteDWord(hPlx, hPlx->addrSpace, dwOffset, data);
}

void P9054_ReadWriteBlockLocal (P9054_HANDLE hPlx, DWORD dwLocalAddr, PVOID buf, 
                    DWORD dwBytes, BOOL fIsRead, P9054_MODE mode)
{
    DWORD dwOffset = hPlx->addrDesc[hPlx->addrSpace].dwMask & dwLocalAddr;
    P9054_SetMode (hPlx, dwLocalAddr);
    P9054_ReadWriteBlock(hPlx, dwOffset, buf, dwBytes, fIsRead, hPlx->addrSpace, mode);
}

void P9054_ReadBlockLocal (P9054_HANDLE hPlx, DWORD dwLocalAddr, PVOID buf, 
                    DWORD dwBytes, P9054_MODE mode)
{
    P9054_ReadWriteBlockLocal (hPlx, dwLocalAddr, buf, dwBytes, TRUE, mode);
}

void P9054_WriteBlockLocal (P9054_HANDLE hPlx, DWORD dwLocalAddr, PVOID buf, 
                     DWORD dwBytes, P9054_MODE mode)
{
    P9054_ReadWriteBlockLocal (hPlx, dwLocalAddr, buf, dwBytes, FALSE, mode);
}

BOOL P9054_IntIsEnabled (P9054_HANDLE hPlx)
{
    if (!hPlx->fUseInt) return FALSE;
    if (!hPlx->Int.hThread) return FALSE;
    return TRUE;
}

VOID P9054_IntHandler (PVOID pData)
{
    P9054_HANDLE hPlx = (P9054_HANDLE) pData;
    P9054_INT_RESULT intResult;
    intResult.dwCounter = hPlx->Int.Int.dwCounter;
    intResult.dwLost = hPlx->Int.Int.dwLost;
    intResult.fStopped = hPlx->Int.Int.fStopped;
    intResult.dwStatusReg = hPlx->Int.Trans[0].Data.Dword;
    hPlx->Int.funcIntHandler(hPlx, &intResult);  
}

BOOL P9054_IntEnable (P9054_HANDLE hPlx, P9054_INT_HANDLER funcIntHandler)
{
    DWORD dwIntStatus;
    DWORD dwAddr;

    if (!hPlx->fUseInt) return FALSE;
    // check if interrupt is already enabled
    if (hPlx->Int.hThread) return FALSE;

    dwIntStatus = P9054_ReadReg (hPlx, P9054_INTCSR);

    BZERO(hPlx->Int.Trans);
    // This is a samlpe of handling interrupts:
    // Two transfer commands are issued. First the value of the interrrupt control/status
    // register is read. Then, a value of ZERO is written.
    // This will cancel interrupts after the first interrupt occurs.
    // When using interrupts, this section will have to change:
    // you must put transfer commands to CANCEL the source of the interrupt, otherwise, the 
    // PC will hang when an interrupt occurs!
    dwAddr = hPlx->addrDesc[P9054_ADDR_REG].dwAddr + P9054_INTCSR;
    hPlx->Int.Trans[0].cmdTrans = hPlx->addrDesc[P9054_ADDR_REG].fIsMemory ? RM_DWORD : RP_DWORD;
    hPlx->Int.Trans[0].dwPort = dwAddr;
    hPlx->Int.Trans[1].cmdTrans = hPlx->addrDesc[P9054_ADDR_REG].fIsMemory ? WM_DWORD : WP_DWORD;
    hPlx->Int.Trans[1].dwPort = dwAddr;
    hPlx->Int.Trans[1].Data.Dword = dwIntStatus & ~(BIT8|BIT10); // put here the data to write to the control register
    hPlx->Int.Int.dwCmds = 2; 
    hPlx->Int.Int.Cmd = hPlx->Int.Trans;
    hPlx->Int.Int.dwOptions |= INTERRUPT_CMD_COPY;
 
    // this calls WD_IntEnable() and creates an interrupt handler thread
    hPlx->Int.funcIntHandler = funcIntHandler;
    if (!InterruptThreadEnable(&hPlx->Int.hThread, hPlx->hWD, &hPlx->Int.Int, P9054_IntHandler, (PVOID) hPlx))
        return FALSE;

    // this physically enables interrupts
    P9054_WriteReg (hPlx, P9054_INTCSR, dwIntStatus | (BIT8|BIT10));

    return TRUE;
}

void P9054_IntDisable (P9054_HANDLE hPlx)
{
    DWORD dwIntStatus;

    if (!hPlx->fUseInt) return;
    if (!hPlx->Int.hThread) return;

    // this disables interrupts
    dwIntStatus = P9054_ReadReg (hPlx, P9054_INTCSR);
    P9054_WriteReg (hPlx, P9054_INTCSR, dwIntStatus & ~(BIT8|BIT10));

    // this calls WD_IntDisable()
    InterruptThreadDisable(hPlx->Int.hThread);
    hPlx->Int.hThread = NULL;
}

P9054_DMA_HANDLE P9054_DMAOpen (P9054_HANDLE hPlx, DWORD dwLocalAddr, PVOID buf, 
    DWORD dwBytes, BOOL fIsRead, P9054_MODE mode, P9054_DMA_CHANNEL dmaChannel)
{
    DWORD dwDMAMODE, dwDMADPR, dwDMALADR;
    DWORD dwChannelShift = (dmaChannel==P9054_DMA_CHANNEL_0) ? 0 : P9054_DMA_CHANNEL_SHIFT;
    BOOL fAutoinc = TRUE;
    P9054_DMA_HANDLE hDma;
    
    hDma = (P9054_DMA_HANDLE) malloc (sizeof(P9054_DMA_STRUCT));
    if (hDma==NULL)
    {
        sprintf( P9054_ErrorString, "Failed allocating memory for dma handle!\n");
        goto Exit;
    }
    BZERO (*hDma);
    hDma->dmaChannel = dmaChannel;
    hDma->dma.dwBytes = dwBytes;
    hDma->dma.pUserAddr = buf; 
    hDma->dma.dwOptions = 0; 
    WD_DMALock (hPlx->hWD, &hDma->dma);
    if (!hDma->dma.hDma) 
    {
        sprintf( P9054_ErrorString, "Failed locking the buffer!\n");
        goto Exit;
    }
    if (hDma->dma.dwPages==1)
    {
        //dma of one page ==> direct dma
        dwDMAMODE = 
            (fAutoinc ? 0 : BIT11) 
            | BIT6 
            | (mode==P9054_MODE_BYTE ? 0 : mode==P9054_MODE_WORD ? BIT0 : (BIT1 | BIT0));
        dwDMADPR = BIT0 | (fIsRead ? BIT3 : 0);
        dwDMALADR = dwLocalAddr;

        P9054_WriteReg (hPlx, P9054_DMAMODE + dwChannelShift, dwDMAMODE);
        P9054_WriteReg (hPlx, P9054_DMAPADR + dwChannelShift, (DWORD) hDma->dma.Page[0].pPhysicalAddr);
        P9054_WriteReg (hPlx, P9054_DMALADR + dwChannelShift, dwDMALADR);
        P9054_WriteReg (hPlx, P9054_DMASIZ + dwChannelShift, hDma->dma.Page[0].dwBytes);
        P9054_WriteReg (hPlx, P9054_DMADPR + dwChannelShift, dwDMADPR);
    }
    else
    {
        DWORD dwAlignShift, dwPageNumber, dwMemoryCopied;
        typedef struct {
            DWORD dwPADR;
            DWORD dwLADR;
            DWORD dwSIZ;
            DWORD dwDPR;
        } DMA_LIST;
        DMA_LIST *pList;

        //dma of more then one page ==> chain dma
        hDma->dmaList.dwBytes = hDma->dma.dwPages * sizeof(DMA_LIST) + 0x10; // includes extra 0x10 bytes for quadword alignment
        hDma->dmaList.pUserAddr = NULL;
        hDma->dmaList.dwOptions = DMA_KERNEL_BUFFER_ALLOC;
        WD_DMALock (hPlx->hWD, &hDma->dmaList);
        if (!hDma->dmaList.hDma)
        {
            sprintf (P9054_ErrorString, "Failed allocating the chain buffer!\n");
            goto Exit;
        }

        //setting chain of dma pages in the memory
        dwMemoryCopied = 0;
        dwAlignShift = 0x10 - (DWORD) hDma->dmaList.pUserAddr & 0xf;  
        // verifcation that bits 0-3 are zero (quadword aligned)
        pList = (DMA_LIST *) ((DWORD) hDma->dmaList.pUserAddr + dwAlignShift);
        for (dwPageNumber=0; dwPageNumber<hDma->dma.dwPages; dwPageNumber++)
        {
            pList[dwPageNumber].dwPADR = (DWORD) hDma->dma.Page[dwPageNumber].pPhysicalAddr;
            pList[dwPageNumber].dwLADR = dwLocalAddr + (fAutoinc ? dwMemoryCopied : 0);
            pList[dwPageNumber].dwSIZ = hDma->dma.Page[dwPageNumber].dwBytes;
            pList[dwPageNumber].dwDPR = 
                ((DWORD) hDma->dmaList.Page[0].pPhysicalAddr + dwAlignShift + sizeof(DMA_LIST)*(dwPageNumber+1))
                | BIT0 | (fIsRead ? BIT3 : 0);
            dwMemoryCopied += hDma->dma.Page[dwPageNumber].dwBytes;
        }
        pList[dwPageNumber - 1].dwDPR |= BIT1; // mark end of chain
    
        dwDMAMODE = (fAutoinc ? 0 : BIT11) 
                    | BIT6
                    | BIT9        // chain transfer
                     | (mode==P9054_MODE_BYTE ? 0 : mode==P9054_MODE_WORD ? BIT0 : (BIT1 | BIT0));
        dwDMADPR = ((DWORD)hDma->dmaList.Page[0].pPhysicalAddr + dwAlignShift) | BIT0; 
        // starting the dma
        P9054_WriteReg (hPlx, P9054_DMAMODE + dwChannelShift, dwDMAMODE);
        P9054_WriteReg (hPlx, P9054_DMADPR + dwChannelShift, dwDMADPR);
    }

    return hDma;

Exit:
    if (hDma!=NULL)
        P9054_DMAClose(hPlx,hDma);
    return NULL;
}

void P9054_DMAClose (P9054_HANDLE hPlx, P9054_DMA_HANDLE hDma)
{
    if (hDma->dma.hDma)
        WD_DMAUnlock(hPlx->hWD, &hDma->dma);
    if (hDma->dmaList.hDma)
        WD_DMAUnlock(hPlx->hWD, &hDma->dmaList);
    free (hDma);
}

BOOL P9054_DMAIsDone (P9054_HANDLE hPlx, P9054_DMA_HANDLE hDma)
{
    return (P9054_ReadByte(hPlx, P9054_ADDR_REG, P9054_DMACSR + 
		hDma->dmaChannel) & BIT4) == BIT4;
}

void P9054_DMAStart (P9054_HANDLE hPlx, P9054_DMA_HANDLE hDma, BOOL fBlocking)
{
    P9054_WriteByte (hPlx, P9054_ADDR_REG, P9054_DMACSR + hDma->dmaChannel, 
		BIT0 | BIT1);

    //Busy wait for plx to finish transfer
    if (fBlocking) 
        while (!P9054_DMAIsDone(hPlx, hDma))
			;
}

BOOL P9054_DMAReadWriteBlock (P9054_HANDLE hPlx, DWORD dwLocalAddr, PVOID buf, 
    DWORD dwBytes, BOOL fIsRead, P9054_MODE mode, P9054_DMA_CHANNEL dmaChannel)
{
    P9054_DMA_HANDLE hDma;
    if (dwBytes==0) 
        return TRUE;

    hDma = P9054_DMAOpen(hPlx, dwLocalAddr, buf, dwBytes, fIsRead, mode, dmaChannel);
    if (!hDma) 
        return FALSE;

    P9054_DMAStart(hPlx, hDma, TRUE);
    P9054_DMAClose(hPlx, hDma);
    return TRUE;
}

BOOL P9054_EEPROMValid(P9054_HANDLE hPlx)
{
    return (P9054_ReadReg(hPlx, P9054_CNTRL) & BIT28)==BIT28;
}

BOOL P9054_EEPROMReadWord(P9054_HANDLE hPlx, DWORD dwOffset, PWORD pwData)
{
	DWORD dwData;
	DWORD dwAddr;

	if (dwOffset % 2)
	{
        sprintf (P9054_ErrorString, "The offset is not even\n");
		return FALSE;
	}
	dwAddr = dwOffset - (dwOffset % 4);

	if (!P9054_EEPROMReadDWord(hPlx, dwAddr, &dwData))
		return FALSE;

	*pwData = (WORD) (dwData >> ((dwOffset % 4)*8));

	return TRUE;
}

BOOL P9054_EEPROMWriteWord(P9054_HANDLE hPlx, DWORD dwOffset, WORD wData)
{
	DWORD dwData;
	DWORD dwAddr;

	dwAddr = dwOffset - (dwOffset % 4);

	if (!P9054_EEPROMReadDWord(hPlx, dwAddr, &dwData))
		return FALSE;

	switch (dwOffset % 4)
	{
	case 0:
		dwData = (dwData & 0xffff0000) | wData;
		break;
	case 2:
		dwData = (dwData & 0x0000ffff) | (wData << 16);
		break;
	default:
        sprintf (P9054_ErrorString, "The offset is not even\n");
		return FALSE;
	}

	return P9054_EEPROMWriteDWord(hPlx, dwAddr, dwData);
}

void P9054_Sleep(P9054_HANDLE hPlx, DWORD dwMicroSeconds)
{
	WD_SLEEP sleep;

    BZERO (sleep);
    sleep.dwMicroSeconds = dwMicroSeconds;
    WD_Sleep( hPlx->hWD, &sleep);
}

BYTE P9054_EEPROMEnable(P9054_HANDLE hPlx, WORD addr)
{
	BYTE old_val;

	old_val = P9054_ReadByte(hPlx, P9054_ADDR_REG, P9054_PROT_AREA);
	
	addr /= 4;
	addr &= 0x7f;

	P9054_WriteByte(hPlx, P9054_ADDR_REG, P9054_PROT_AREA, (BYTE)addr);
	P9054_Sleep(hPlx, 10000);

	return old_val * 4; //expand from dwords to bytes
}

void P9054_EEPROMDataReadWrite(P9054_HANDLE hPlx, BOOL fIsRead, PDWORD pdwData)
{
	WD_PCI_CONFIG_DUMP pciCnf;

	BZERO (pciCnf);
    pciCnf.pciSlot = hPlx->pciSlot;
    pciCnf.pBuffer = pdwData;
    pciCnf.dwOffset = P9054_VPD_DATA;
    pciCnf.dwBytes = 4;
    pciCnf.fIsRead = fIsRead;
    WD_PciConfigDump(hPlx->hWD,&pciCnf);
}

void P9054_EEPROMAddrReadWrite(P9054_HANDLE hPlx, BOOL fIsRead, PWORD pwAddr)
{
	WD_PCI_CONFIG_DUMP pciCnf;

	BZERO (pciCnf);
    pciCnf.pciSlot = hPlx->pciSlot;
    pciCnf.pBuffer = pwAddr;
    pciCnf.dwOffset = P9054_VPD_ADDR;
    pciCnf.dwBytes = 2;
    pciCnf.fIsRead = fIsRead;
    WD_PciConfigDump(hPlx->hWD,&pciCnf);
}

BOOL P9054_EEPROMReadDWord(P9054_HANDLE hPlx, DWORD dwOffset, PDWORD pdwData)
{
	WORD wVal;
	WORD wAddr;
	int i ;
	BOOL fEnd = FALSE ;

	if (dwOffset % 4)
	{
        sprintf (P9054_ErrorString, "The offset is not a multiple of 4\n");
		return FALSE;
	}
	wAddr = (((WORD)dwOffset) & (~BIT15)) ;
 
	
	P9054_EEPROMAddrReadWrite(hPlx, FALSE, &wAddr);

    P9054_Sleep(hPlx, 10000);

	for (i=0; !fEnd && i<100; i++)
	{
		P9054_EEPROMAddrReadWrite(hPlx, TRUE, &wVal);
		if (wVal & BIT15)
			fEnd = TRUE;
		P9054_Sleep(hPlx, 10000);
	}
	
	if (i==100)
	{
        sprintf (P9054_ErrorString, "Acknoledge to EEPROM read was not recived\n");
		return FALSE;
	}

	P9054_EEPROMDataReadWrite(hPlx, TRUE, pdwData);

	return TRUE;
}

BOOL P9054_EEPROMWriteDWord(P9054_HANDLE hPlx, DWORD dwOffset, DWORD dwData)
{
	DWORD dwReadback;
	WORD wAddr;
	WORD wVal;
	int i;
	BOOL fRet;
	BOOL fEnd = FALSE ;
	BOOL fReadOk = FALSE;
	BYTE bEnableOffset;


	if (dwOffset % 4)
	{
        sprintf (P9054_ErrorString, "The offset is not a multiple of 4\n");
		return FALSE;
	}
    wAddr = (WORD)dwOffset;
	bEnableOffset = P9054_EEPROMEnable(hPlx, wAddr);
	wAddr = wAddr | BIT15;

	P9054_EEPROMDataReadWrite(hPlx, FALSE, &dwData);

	P9054_EEPROMAddrReadWrite(hPlx, FALSE, &wAddr);

    P9054_Sleep(hPlx, 10000);
	
	for (i=0; !fEnd && i<100 ;i++)
	{
		P9054_EEPROMAddrReadWrite(hPlx, TRUE, &wVal);
		if ((wVal & BIT15) == 0)
			fEnd = TRUE;
		P9054_Sleep(hPlx, 10000);
	}

	fReadOk = P9054_EEPROMReadDWord(hPlx, dwOffset, &dwReadback);

	if (fReadOk && dwReadback==dwData)
		fRet = TRUE;
	else
	{
		fRet = FALSE;
		if (fReadOk)
			sprintf (P9054_ErrorString, "Write 0x%08x, Read 0x%08x\n",dwData, dwReadback);
		else
			sprintf (P9054_ErrorString, "Error reading EEPROM\n");
	}
	P9054_EEPROMEnable(hPlx, bEnableOffset); 
	return fRet;
}
