"""__NATIVE__
#include "mbed.h"
#include "TextLCD.h"

TextLCD lcd(24, 25, 26, 27, 28, 29, 30);
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
"""


def lcd_print(s):
    """__NATIVE__
    pPmObj_t ps = C_NULL;
    uint8_t *s = C_NULL;
    PmReturn_t retval;

    /* If wrong number of args, throw type exception */
    if (NATIVE_GET_NUM_ARGS() != 1)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Get the arg, throw type exception if needed */
    ps = NATIVE_GET_LOCAL(0);
    if (OBJ_GET_TYPE(ps) != OBJ_TYPE_STR)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Get a pointer to the string */
    s = ((pPmString_t)ps)->val;

    /* Print the string on the lcd */
    lcd.printf((char *)s);
    
    /* Return none obj on stack */
    NATIVE_SET_TOS(PM_NONE);

    return PM_RET_OK;
    """
    pass


def led_set(n):
    """__NATIVE__
    pPmObj_t pn;
    int32_t n;
    PmReturn_t retval;

    /* If wrong number of args, raise TypeError */
    if (NATIVE_GET_NUM_ARGS() != 1)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* If arg is not an int, raise TypeError */
    pn = NATIVE_GET_LOCAL(0);
    if (OBJ_GET_TYPE(pn) != OBJ_TYPE_INT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Get int value from the arg */
    n = ((pPmInt_t)pn)->val;

    /* Clear all and set the desired LEDs */
    led1 = (n & 1); 
    led2 = (n & 2); 
    led3 = (n & 4); 
    led4 = (n & 8); 

    NATIVE_SET_TOS(PM_NONE);
    return PM_RET_OK;
    """
    pass

# :mode=c:
