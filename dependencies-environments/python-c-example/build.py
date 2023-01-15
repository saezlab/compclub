from setuptools.extension import Extension


linked_pyext = Extension(
    "linked_pyext.add",
    sources=["src/linked_pyext.c"],
    define_macros=[("PY_SSIZE_T_CLEAN",)],
)


def build(setup_kwargs):
    """
    This is a callback for poetry used to hook in our extensions.
    """

    setup_kwargs.update(
        {
            "ext_modules": [linked_pyext],
        }
    )
