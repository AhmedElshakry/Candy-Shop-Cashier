#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define CANDY_KINDS 6U
#define BASKET_MAX  8U
#define NAME_LEN    16U

typedef struct
{
    char     name[NAME_LEN];
    uint16_t price;     /* piastres */
    uint16_t stock;     /* how many are left on the shelf */
    uint16_t sold;      /* how many we sold today */
} Candy_t;

typedef struct
{
    uint8_t candyId;
    uint8_t qty;
} Line_t;

static Candy_t shelf[CANDY_KINDS];
static Line_t basket[BASKET_MAX];
static uint8_t basketLines;
static uint32_t cashDrawer;

/* Prototypes */
static void openShop(void);
static void showShelf(void);
static void addToBasket(void);
static void removeFromBasket(void);
static uint32_t basketTotal(void);
static void showBasket(void);
static void checkout(void);
static void giveChange(uint32_t change);
static uint8_t bestseller(void);
static void dayReport(void);


/* =========================================================
   Open shop
   ========================================================= */
static void openShop(void)
{
    memset(shelf, 0, sizeof(shelf));
    memset(basket, 0, sizeof(basket));

    basketLines = 0U;
    cashDrawer = 0U;

    strcpy(shelf[0].name, "Chocolate");
    shelf[0].price = 100U;
    shelf[0].stock = 20U;
    shelf[0].sold = 0U;

    strcpy(shelf[1].name, "Gummy");
    shelf[1].price = 75U;
    shelf[1].stock = 20U;
    shelf[1].sold = 0U;

    strcpy(shelf[2].name, "Lollipop");
    shelf[2].price = 50U;
    shelf[2].stock = 20U;
    shelf[2].sold = 0U;

    strcpy(shelf[3].name, "Caramel");
    shelf[3].price = 125U;
    shelf[3].stock = 20U;
    shelf[3].sold = 0U;

    strcpy(shelf[4].name, "Toffee");
    shelf[4].price = 150U;
    shelf[4].stock = 20U;
    shelf[4].sold = 0U;

    strcpy(shelf[5].name, "Marshmallow");
    shelf[5].price = 200U;
    shelf[5].stock = 20U;
    shelf[5].sold = 0U;

    printf("\nShop opened successfully.\n");
}


/* =========================================================
   Show shelf
   ========================================================= */
static void showShelf(void)
{
    uint8_t i;

    printf("\n==============================\n");
    printf("          CANDY SHELF\n");
    printf("==============================\n");

    for (i = 0U; i < CANDY_KINDS; i++)
    {
        if (shelf[i].stock == 0U)
        {
            printf("%u. %-15s Price: %u piastres  Stock: SOLD OUT\n",
                   (unsigned int)(i + 1U),
                   shelf[i].name,
                   (unsigned int)shelf[i].price);
        }
        else
        {
            printf("%u. %-15s Price: %u piastres  Stock: %u\n",
                   (unsigned int)(i + 1U),
                   shelf[i].name,
                   (unsigned int)shelf[i].price,
                   (unsigned int)shelf[i].stock);
        }
    }

    printf("==============================\n");
}


/* =========================================================
   Add candy to basket
   ========================================================= */
static void addToBasket(void)
{
    unsigned int id;
    unsigned int qty;
    uint8_t index;
    uint8_t i;

    printf("\nEnter candy ID (1-%u): ", CANDY_KINDS);

    if (scanf("%u", &id) != 1)
    {
        printf("Invalid candy ID.\n");

        while (getchar() != '\n')
        {
        }

        return;
    }

    if ((id < 1U) || (id > CANDY_KINDS))
    {
        printf("Invalid candy ID.\n");
        return;
    }

    printf("Enter quantity: ");

    if (scanf("%u", &qty) != 1)
    {
        printf("Invalid quantity.\n");

        while (getchar() != '\n')
        {
        }

        return;
    }

    if ((qty == 0U) || (qty > 255U))
    {
        printf("Invalid quantity.\n");
        return;
    }

    index = (uint8_t)(id - 1U);

    if (qty > (unsigned int)shelf[index].stock)
    {
        printf("Not enough stock.\n");
        printf("Available: %u\n",
               (unsigned int)shelf[index].stock);
        return;
    }

    for (i = 0U; i < basketLines; i++)
    {
        if (basket[i].candyId == index)
        {
            if (qty > (255U - (unsigned int)basket[i].qty))
            {
                printf("Basket quantity is too large.\n");
                return;
            }

            basket[i].qty =
                (uint8_t)((unsigned int)basket[i].qty + qty);

            shelf[index].stock =
                (uint16_t)((unsigned int)shelf[index].stock - qty);

            printf("Added to existing basket line.\n");
            return;
        }
    }

    if (basketLines >= BASKET_MAX)
    {
        printf("Basket is full.\n");
        return;
    }

    basket[basketLines].candyId = index;
    basket[basketLines].qty = (uint8_t)qty;
    basketLines++;

    shelf[index].stock =
        (uint16_t)((unsigned int)shelf[index].stock - qty);

    printf("Candy added to basket.\n");
}


/* =========================================================
   Remove candy from basket
   ========================================================= */
static void removeFromBasket(void)
{
    unsigned int lineNumber;
    unsigned int qty;
    uint8_t index;
    uint8_t line;
    uint8_t i;

    if (basketLines == 0U)
    {
        printf("\nBasket is empty.\n");
        return;
    }

    showBasket();

    printf("\nEnter basket line to remove: ");

    if (scanf("%u", &lineNumber) != 1)
    {
        printf("Invalid line number.\n");

        while (getchar() != '\n')
        {
        }

        return;
    }

    if ((lineNumber < 1U) ||
        (lineNumber > (unsigned int)basketLines))
    {
        printf("Invalid line number.\n");
        return;
    }

    line = (uint8_t)(lineNumber - 1U);
    index = basket[line].candyId;

    printf("Enter quantity to remove (1-%u): ",
           (unsigned int)basket[line].qty);

    if (scanf("%u", &qty) != 1)
    {
        printf("Invalid quantity.\n");

        while (getchar() != '\n')
        {
        }

        return;
    }

    if ((qty == 0U) ||
        (qty > (unsigned int)basket[line].qty))
    {
        printf("Invalid quantity.\n");
        return;
    }

    shelf[index].stock =
        (uint16_t)((unsigned int)shelf[index].stock + qty);

    basket[line].qty =
        (uint8_t)((unsigned int)basket[line].qty - qty);

    if (basket[line].qty == 0U)
    {
        for (i = line; i < (uint8_t)(basketLines - 1U); i++)
        {
            basket[i] = basket[i + 1U];
        }

        memset(&basket[basketLines - 1U],
               0,
               sizeof(basket[basketLines - 1U]));

        basketLines--;
    }

    printf("Item removed from basket.\n");
}


/* =========================================================
   Calculate basket total
   ========================================================= */
static uint32_t basketTotal(void)
{
    uint8_t i;
    uint8_t candyId;
    uint32_t total = 0U;

    for (i = 0U; i < basketLines; i++)
    {
        candyId = basket[i].candyId;

        total +=
            (uint32_t)shelf[candyId].price *
            (uint32_t)basket[i].qty;
    }

    return total;
}


/* =========================================================
   Show basket
   ========================================================= */
static void showBasket(void)
{
    uint8_t i;
    uint8_t candyId;
    uint32_t lineTotal;

    printf("\n==============================\n");
    printf("           BASKET\n");
    printf("==============================\n");

    if (basketLines == 0U)
    {
        printf("Basket is empty.\n");
        printf("==============================\n");
        return;
    }

    for (i = 0U; i < basketLines; i++)
    {
        candyId = basket[i].candyId;

        lineTotal =
            (uint32_t)shelf[candyId].price *
            (uint32_t)basket[i].qty;

        printf("%u. %-15s Qty: %u  Total: %lu piastres\n",
               (unsigned int)(i + 1U),
               shelf[candyId].name,
               (unsigned int)basket[i].qty,
               (unsigned long)lineTotal);
    }

    printf("------------------------------\n");
    printf("Basket total: %lu piastres\n",
           (unsigned long)basketTotal());

    printf("==============================\n");
}


/* =========================================================
   Checkout
   ========================================================= */
static void checkout(void)
{
    uint32_t total;
    uint32_t paid;
    uint8_t i;
    uint8_t candyId;

    if (basketLines == 0U)
    {
        printf("\nBasket is empty.\n");
        return;
    }

    total = basketTotal();

    showBasket();

    printf("\nTotal to pay: %lu piastres\n",
           (unsigned long)total);

    printf("Enter amount paid: ");

    if (scanf("%lu", (unsigned long *)&paid) != 1)
    {
        printf("Invalid payment.\n");

        while (getchar() != '\n')
        {
        }

        return;
    }

    if (paid < total)
    {
        printf("\nPayment is insufficient.\n");
        printf("Required: %lu\n",
               (unsigned long)total);
        printf("Paid:     %lu\n",
               (unsigned long)paid);
        printf("Transaction cancelled.\n");

        return;
    }

    for (i = 0U; i < basketLines; i++)
    {
        candyId = basket[i].candyId;

        shelf[candyId].sold =
            (uint16_t)((unsigned int)shelf[candyId].sold +
                       (unsigned int)basket[i].qty);
    }

    cashDrawer += total;

    if (paid > total)
    {
        giveChange(paid - total);
    }
    else
    {
        printf("No change.\n");
    }

    memset(basket, 0, sizeof(basket));
    basketLines = 0U;

    printf("Checkout completed successfully.\n");
}


/* =========================================================
   Give change
   ========================================================= */
static void giveChange(uint32_t change)
{
    static const uint16_t coins[] =
    {
        500U,
        200U,
        100U,
        50U,
        25U
    };

    uint8_t i;
    uint32_t count;

    printf("\nChange: %lu piastres\n",
           (unsigned long)change);

    printf("Coins:\n");

    for (i = 0U; i < 5U; i++)
    {
        count = change / coins[i];

        if (count > 0U)
        {
            printf("%u x %u piastres\n",
                   (unsigned int)count,
                   (unsigned int)coins[i]);

            change %= coins[i];
        }
    }

    if (change != 0U)
    {
        printf("WARNING: %lu piastres cannot be represented ",
               (unsigned long)change);
        printf("using available coins.\n");
    }
}


/* =========================================================
   Find bestseller
   ========================================================= */
static uint8_t bestseller(void)
{
    uint8_t i;
    uint8_t best = 0U;

    for (i = 1U; i < CANDY_KINDS; i++)
    {
        if (shelf[i].sold > shelf[best].sold)
        {
            best = i;
        }
    }

    return best;
}


/* =========================================================
   Day report
   ========================================================= */
static void dayReport(void)
{
    uint8_t i;
    uint8_t best;

    printf("\n====================================\n");
    printf("            DAY REPORT\n");
    printf("====================================\n");

    printf("Cash drawer: %lu piastres\n",
           (unsigned long)cashDrawer);

    printf("\nCandy sales:\n");

    for (i = 0U; i < CANDY_KINDS; i++)
    {
        printf("%-15s Sold: %u  Stock: %u\n",
               shelf[i].name,
               (unsigned int)shelf[i].sold,
               (unsigned int)shelf[i].stock);
    }

    best = bestseller();

    printf("\nBestseller: %s\n",
           shelf[best].name);

    printf("Units sold: %u\n",
           (unsigned int)shelf[best].sold);

    printf("====================================\n");
}