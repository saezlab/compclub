from setuptools.extension import Extension


linked_pyext = Extension(
    "linked_pyext._linkedext",
    sources = ["src/linked_pyextmodule.c"],
    define_macros = [("PY_SSIZE_T_CLEAN",)],
    libraries = ["tidy", "curl"]
)


def build(setup_kwargs):
    """
    This is a callback for poetry used to hook in our extensions.
    """

    setup_kwargs.update(
        {
            "ext_modules": [linked_pyext]
        }
    )
