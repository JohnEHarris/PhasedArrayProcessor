#ifndef _PCI_REGS_H_
#define _PCI_REGS_H_

// PCI register definitions 
enum {
    PCI_IDR  = 0x00,
    PCI_CR   = 0x04,
    PCI_SR   = 0x06,
    PCI_REV  = 0x08,
    PCI_CCR  = 0x09,
    PCI_LSR  = 0x0c,
    PCI_LTR  = 0x0d,
    PCI_HTR  = 0x0e,
    PCI_BISTR= 0x0f,
    PCI_BAR0 = 0x10,
    PCI_BAR1 = 0x14,
    PCI_BAR2 = 0x18,
    PCI_BAR3 = 0x1c,
    PCI_BAR4 = 0x20,
    PCI_BAR5 = 0x24,
    PCI_CIS  = 0x28,
    PCI_SVID = 0x2c,
    PCI_SID  = 0x2e,
    PCI_ERBAR= 0x30,
    PCI_ILR  = 0x3c,
    PCI_IPR  = 0x3d,
    PCI_MGR  = 0x3e,
    PCI_MLR  = 0x3f
};

enum {
	AD_PCI_BAR0 = 0,
	AD_PCI_BAR1 = 1,
	AD_PCI_BAR2 = 2,
	AD_PCI_BAR3 = 3,
	AD_PCI_BAR4 = 4,
	AD_PCI_BAR5 = 5,
	AD_PCI_BAR_EPROM = 6,
	AD_PCI_BARS = 7,
};

#endif
