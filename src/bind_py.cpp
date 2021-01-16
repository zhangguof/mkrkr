#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <iostream>
#include <string>
using namespace boost::python;

extern const char* GetCurPath();

object main_module;
object main_namespace;
object g_py_update;
char python_home[256];

const char* py_init_script="import sys\n"
                            "sys.path.insert(0,'./python')\n"
                            "import py_init\n"
                            "py_init.init()\n";

//export function
std::string hello()
{
    return "hello world";
}

const char* py_version()
{
    return Py_GetVersion();
}

void regist_update(object update_func)
{
    g_py_update = update_func;
}

//init
BOOST_PYTHON_MODULE(engine)
{
    def("hello",hello,"a hello test func.");
    def("py_version",py_version,"python version");
    def("regist_update",regist_update,"python update func.");
}
void py_init_ext_moudles();
static void py_init_modules()
{
    //extension as build in.
    py_init_ext_moudles();
    //others
    initengine();

}

static void run_init_script()
{
    main_module = import("__main__");
    main_namespace = main_module.attr("__dict__");
    try
    {
        /* code */
        object ignored = exec(py_init_script,main_namespace);
    }
    catch(const error_already_set& e)
    {
        PyErr_Print();
    }
    
    
}

void py_init(char *program_name)
{
	Py_SetProgramName(program_name);  /* optional but recommended */
    std::string py_home = GetCurPath();
    py_home += "python/";
    strncpy(python_home,py_home.c_str(),sizeof(python_home)-1);
    printf("set pythone home:%s\n",python_home);
    Py_SetPythonHome(python_home);

	Py_NoSiteFlag = 1;
  	Py_Initialize();
    py_init_modules();
    run_init_script();
    //test

    // if(!g_py_update.is_none())
    //     g_py_update(100);
    // exit(0);
}

void py_exit()
{
    Py_Finalize();
}

void py_do_update(unsigned int cur_tick)
{
    if(g_py_update.is_none()) return;
    g_py_update(cur_tick);
}
