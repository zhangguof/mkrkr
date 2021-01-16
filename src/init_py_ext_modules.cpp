
#include "python.h"

PyMODINIT_FUNC initoperator(void);
PyMODINIT_FUNC initcStringIO(void);
PyMODINIT_FUNC inititertools(void);
PyMODINIT_FUNC init_heapq(void);
PyMODINIT_FUNC initbinascii(void);

void py_init_ext_moudles()
{
    initoperator();
    initcStringIO();
    inititertools();
    init_heapq();
    initbinascii();

}