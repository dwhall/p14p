#include "pm.h"
#include "main_img.h"


int
main(void)
{
    PmReturn_t retval;

    /* Init PyMite */
    retval = pm_init(MEMSPACE_PROG, (uint8_t *)usrlib_img);
    PM_RETURN_IF_ERROR(retval);

    /* Run the sample program */
    retval = pm_run((uint8_t *)"main");

    return (int)retval;
}
