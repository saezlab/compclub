#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyObject *
linkedpyext_add(PyObject *self, PyObject *args)
{
    const long a, b;
    long result;

    if (!PyArg_ParseTuple(args, "l", &a, &b))
        return NULL;
    result = a + b;
    return PyLong_FromLong(result);
}


static PyMethodDef LinkedPyextMethods[] = {
    ...
    {"add", linkedpyext_add, METH_VARARGS,
        "Add two integers."},
        ...
        {NULL, NULL, 0, NULL}
};


static struct PyModuleDef linked_pyextmodule = {
    PyModuleDef_HEAD_INIT,
    "linked_pyext",
    NULL,
    -1,
    LinkedPyextMethods
};


PyMODINIT_FUNC
PyInit_linked_pyext(void)
{
    return PyModule_Create(&linked_pyextmodule);
}
