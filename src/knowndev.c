/** @file knowndev.c
 *
 * @brief Implements known device linked-list, used for apci1710_lookup_board_by_index
 *
 * The index given to apci1710_lookup_board_by_index is supposed to be also the minor number
 * of the device. For drivers that manage only one type of board a simple algorithme works
 * good to implement that. Anyway for drivers that manage several types of device this is
 * things go more complicated.
 *
 * The index/minor number is actually the order of detection by the OS of the boards.
 * For one type of board it is deterministic as the PCI slots are always scanned in the same
 * order by the kernel/hardware.
 * Anyway, when a driver want to manage several types of boards, the order in discover the board,
 * whereas probably still deterministic from one boot to another, is a lot less evident.
 *
 * To handle this case, the simple way is to implements a linked list that will memorize
 * the order of discovery of the boards. It is named "known_boards"
 *
 */

#include "apci1710-private.h"
#include "apci1710-kapi.h"
#include "knowndev.h"

EXPORT_SYMBOL(apci1710_lookup_board_by_index);

/**@def EXPORT_NO_SYMBOLS
 * Function in this file are not exported.
 */
EXPORT_NO_SYMBOLS;


//------------------------------------------------------------------------------
static struct pci_dev * known_dev = NULL;
//------------------------------------------------------------------------------

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,11)
	static spinlock_t spinlock_known_dev = SPIN_LOCK_UNLOCKED;
#else
	DEFINE_SPINLOCK(spinlock_known_dev);
#endif

#define KNOWNDEV_LIST_LOCK(__flag) spin_lock_irqsave(&spinlock_known_dev,__flag);
#define KNOWNDEV_LIST_UNLOCK(__flag)  spin_unlock_irqrestore(&spinlock_known_dev,__flag);
//------------------------------------------------------------------------------
/** get tail of the list */
static __inline__ struct pci_dev * GET_LAST(void)
{
	struct pci_dev * current_dev = known_dev;
	while(current_dev && APCI1710_PRIVDATA(current_dev)->next)
	{
		current_dev = APCI1710_PRIVDATA(current_dev)->next;
	}
	return current_dev;
}
//------------------------------------------------------------------------------
void apci1710_known_dev_append(struct pci_dev * pdev)
{
	unsigned long irqstate = 0;
	KNOWNDEV_LIST_LOCK(irqstate);
	{
		struct pci_dev * last = GET_LAST();

		if (!last)
		{
			/* no entry yet */
			known_dev = pdev;
			APCI1710_PRIVDATA(pdev)->previous = NULL;
			APCI1710_PRIVDATA(pdev)->next = NULL;
			KNOWNDEV_LIST_UNLOCK(irqstate);
			return;
		}

		/* append */
		APCI1710_PRIVDATA(last)->next = pdev;
		APCI1710_PRIVDATA(pdev)->previous = last;
	}
	KNOWNDEV_LIST_UNLOCK(irqstate);
}
//------------------------------------------------------------------------------
void apci1710_known_dev_remove(struct pci_dev * pdev)
{
	unsigned long irqstate = 0;
	KNOWNDEV_LIST_LOCK(irqstate);
	{
		if (APCI1710_PRIVDATA(pdev)->next)
			APCI1710_PRIVDATA(APCI1710_PRIVDATA(pdev)->next)->previous = APCI1710_PRIVDATA(pdev)->previous;
		if (APCI1710_PRIVDATA(pdev)->previous)
			APCI1710_PRIVDATA(APCI1710_PRIVDATA(pdev)->previous)->next = APCI1710_PRIVDATA(pdev)->next;
	}
	KNOWNDEV_LIST_UNLOCK(irqstate);
}
//------------------------------------------------------------------------------
/* implementing this becomes quite trivial now */
struct pci_dev * apci1710_lookup_board_by_index(unsigned int index)
{
	unsigned long irqstate = 0;
	KNOWNDEV_LIST_LOCK(irqstate);
	{
		struct pci_dev * current_dev = known_dev;
		int i = 0;

		while ( current_dev )
		{
			if (i == index)
			{
				KNOWNDEV_LIST_UNLOCK(irqstate);
				return current_dev;
			}
			i++;
			current_dev = APCI1710_PRIVDATA(current_dev)->next;
		}
	}
	KNOWNDEV_LIST_UNLOCK(irqstate);
	return NULL;
}
