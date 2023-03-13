#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <tidy.h>
#include <tidybuffio.h>
#include <curl/curl.h>

/*
 * Largely based on: https://curl.se/libcurl/c/htmltidy.html
 * See also: https://www.html-tidy.org/developer/
 */

/* curl write callback, to fill tidy's input buffer...  */
uint write_cb(char *in, uint size, uint nmemb, TidyBuffer *out)
{
    uint r;
    r = size * nmemb;
    tidyBufAppend(out, in, r);
    return r;
}


TidyBuffer download(char *url)
{
    CURL *curl;
    char curl_errbuf[CURL_ERROR_SIZE];
    TidyBuffer docbuf = {0};
    int err;

    tidyBufInit(&docbuf);

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_errbuf);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &docbuf);

    err = curl_easy_perform(curl);

    if(!err) {
        fprintf(stderr, "%s\n", curl_errbuf);
    }

    /* clean-up */
    curl_easy_cleanup(curl);

    return docbuf;
}


int clean(TidyBuffer docbuf, char *outfile)
{
    TidyDoc tdoc;
    TidyBuffer output = {0};
    TidyBuffer tidy_errbuf = {0};
    int err;

    tdoc = tidyCreate();
    tidyOptSetBool(tdoc, TidyForceOutput, yes); /* try harder */
    tidyOptSetInt(tdoc, TidyWrapLen, 4096);
    tidySetErrorBuffer(tdoc, &tidy_errbuf);

    err = tidyParseBuffer(tdoc, &docbuf); /* parse the input */
    if(err >= 0) {
        err = tidyCleanAndRepair(tdoc); /* fix any problems */
        if(err >= 0) {
            err = tidyRunDiagnostics(tdoc); /* load tidy error buffer */
            if(err >= 0) {
                fprintf(stderr, "%s\n", tidy_errbuf.bp); /* show errors */
            }
        }
    }

    tidySaveBuffer(tdoc, &output);

    if(strlen(outfile)){
        printf("Saving clean HTML into `%s`.\n", outfile);
        tidySaveFile(tdoc, outfile);
    }

    tidyBufFree(&docbuf);
    tidyBufFree(&tidy_errbuf);
    tidyRelease(tdoc);

    return err;
}


static PyObject *
download_html(PyObject *self, PyObject *args)
{
    char *url;
    char *outfile = "";
    TidyBuffer docbuf;
    int err;

    if (!PyArg_ParseTuple(args, "s|s", &url, &outfile))
        return NULL;

    docbuf = download(url);
    err = clean(docbuf, outfile);

    return PyLong_FromLong(err);
}


static PyMethodDef LinkedPyextMethods[] = {
    {"download_html", (PyCFunction)download_html, METH_VARARGS,
        "Download an HTML document."},
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

