#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyObject *
linkedpyext_add(PyObject *self, PyObject *args)
{
    const long a, b;
    long result;

    if (!PyArg_ParseTuple(args, "ll", &a, &b))
        return NULL;
    result = a + b;
    printf("%d + %d = %d\n", a, b, result);
    return PyLong_FromLong(result);
}


static PyMethodDef LinkedPyextMethods[] = {
    {"add", (PyCFunction)linkedpyext_add, METH_VARARGS, "Add two integers."},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef linked_pyextmodule = {
    PyModuleDef_HEAD_INIT,
    "_linkedext",
    "Linked Python extension.",
    -1,
    LinkedPyextMethods
};


PyMODINIT_FUNC
PyInit__linkedext(void)
{
    return PyModule_Create(&linked_pyextmodule);
}

