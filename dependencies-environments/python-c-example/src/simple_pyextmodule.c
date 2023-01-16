#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyObject *
add(PyObject *self, PyObject *args)
{
    const long a, b;
    long result;

    if (!PyArg_ParseTuple(args, "ll", &a, &b))
        return NULL;
    result = a + b;
    printf("%d + %d = %d\n", a, b, result);
    return PyLong_FromLong(result);
}


static PyMethodDef PyextMethods[] = {
    {"add", (PyCFunction)add, METH_VARARGS, "Add two integers."},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef simple_pyextmodule = {
    PyModuleDef_HEAD_INIT,
    "_simpleext",
    "Minimal Python extension.",
    -1,
    PyextMethods
};


PyMODINIT_FUNC
PyInit__simpleext(void)
{
    return PyModule_Create(&simple_pyextmodule);
}

