// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef PUBLIC_FPDF_DATAAVAIL_H_
#define PUBLIC_FPDF_DATAAVAIL_H_

#include <stddef.h>  // For size_t.

#include "fpdfview.h"

#define PDF_LINEARIZATION_UNKNOWN -1
#define PDF_NOT_LINEARIZED 0
#define PDF_LINEARIZED 1

#define PDF_DATA_ERROR -1
#define PDF_DATA_NOTAVAIL 0
#define PDF_DATA_AVAIL 1

#define PDF_FORM_ERROR -1
#define PDF_FORM_NOTAVAIL 0
#define PDF_FORM_AVAIL 1
#define PDF_FORM_NOTEXIST 2

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Interface: FX_FILEAVAIL
 *          Interface for checking whether the section of the file is available.
 */
typedef struct _FX_FILEAVAIL {
    /**
     * Version number of the interface. Currently must be 1.
     */
    int version;

    /**
     * Method: IsDataAvail
     *      Report whether the specified data section is available. A section is
     * available only if all bytes in the section is available.
     * Interface Version:
     *      1
     * Implementation Required:
     *      Yes
     * Parameters:
     *      pThis       -   Pointer to the interface structure itself.
     *      offset      -   The offset of the data section in the file.
     *      size        -   The size of the data section
     * Return Value:
     *      true means the specified data section is available.
     * Comments:
     *      Called by Foxit SDK to check whether the data section is ready.
     */
    FPDF_BOOL (*IsDataAvail)(struct _FX_FILEAVAIL *pThis, size_t offset, size_t size);
} FX_FILEAVAIL;

typedef void *FPDF_AVAIL;

/**
* Function: FPDFAvail_Create
*           Create a document availability provider.
*
* Parameters:
*           file_avail  -   Pointer to file availability interface to check
* availability of file data.
*           file        -   Pointer to a file access interface for reading data
* from file.
* Return value:
*           A handle to the document availability provider. NULL for error.
* Comments:
*           Application must call FPDFAvail_Destroy when done with the
* availability provider.
*/
DLLEXPORT FPDF_AVAIL STDCALL FPDFAvail_Create(FX_FILEAVAIL *file_avail,
                                              FPDF_FILEACCESS *file);

/**
* Function: FPDFAvail_Destroy
*           Destroy a document availibity provider.
*
* Parameters:
*           avail       -   Handle to document availability provider returned by
* FPDFAvail_Create
* Return Value:
*           None.
*/
DLLEXPORT void STDCALL FPDFAvail_Destroy(FPDF_AVAIL avail);

/**
 * Interface: FX_DOWNLOADHINTS
 *          Download hints interface. Used to receive hints for further
 * downloading.
 */
typedef struct _FX_DOWNLOADHINTS {
    /**
     * Version number of the interface. Currently must be 1.
     */
    int version;

    /**
     * Method: AddSegment
     *      Add a section to be downloaded.
     * Interface Version:
     *      1
     * Implementation Required:
     *      Yes
     * Parameters:
     *      pThis       -   Pointer to the interface structure itself.
     *      offset      -   The offset of the hint reported to be downloaded.
     *      size        -   The size of the hint reported to be downloaded.
     * Return Value:
     *      None.
     * Comments:
     *      Called by Foxit SDK to report some downloading hints for download
     * manager.
     *      The position and size of section may be not accurate, part of the
     * section might be already available.
     *      The download manager must deal with that to maximize download
     * efficiency.
     */
    void (*AddSegment)(struct _FX_DOWNLOADHINTS *pThis,
                       size_t offset,
                       size_t size);
} FX_DOWNLOADHINTS;

/**
* Function: FPDFAvail_IsDocAvail
*           Check whether the document is ready for loading, if not, get
* download hints.
*
* Parameters:
*           avail       -   Handle to document availability provider returned by
* FPDFAvail_Create
*           hints       -   Pointer to a download hints interface, receiving
* generated hints
* Return value:
*           PDF_DATA_ERROR: A common error is returned. It can't tell
*                           whehter data are availabe or not.
*           PDF_DATA_NOTAVAIL: Data are not yet available.
*           PDF_DATA_AVAIL: Data are available.
* Comments:
*           Applications should call this function whenever new data arrived,
*           and process all the generated download hints if any, until the
*           function returns PDF_DATA_ERROR or PDF_DATA_AVAIL. Then
*           applications can call FPDFAvail_GetDocument() to get a document
*           handle.
*/
DLLEXPORT int STDCALL
FPDFAvail_IsDocAvail(FPDF_AVAIL avail, FX_DOWNLOADHINTS *hints);

/**
* Function: FPDFAvail_GetDocument
*           Get document from the availability provider.
*
* Parameters:
*           avail       -   Handle to document availability provider returned by
* FPDFAvail_Create
*     password  -   Optional password for decrypting the PDF file.
* Return value:
*           Handle to the document.
* Comments:
*           After FPDFAvail_IsDocAvail() returns TRUE, the application should
* call this function to
*           get the document handle. To close the document, use
* FPDF_CloseDocument function.
*/
DLLEXPORT FPDF_DOCUMENT STDCALL FPDFAvail_GetDocument(FPDF_AVAIL avail,
                                                      FPDF_BYTESTRING password);

/**
* Function: FPDFAvail_GetFirstPageNum
*           Get page number for the first available page in a linearized PDF
*
* Parameters:
*           doc         -   A document handle returned by FPDFAvail_GetDocument
* Return Value:
*           Zero-based index for the first available page.
* Comments:
*           For most linearized PDFs, the first available page would be just the
* first page, however,
*           some PDFs might make other page to be the first available page.
*           For non-linearized PDF, this function will always return zero.
*/
DLLEXPORT int STDCALL FPDFAvail_GetFirstPageNum(FPDF_DOCUMENT doc);

/**
* Function: FPDFAvail_IsPageAvail
*           Check whether a page is ready for loading, if not, get download
* hints.
*
* Parameters:
*           avail       -   Handle to document availability provider returned by
* FPDFAvail_Create
*           page_index  -   Index number of the page. 0 for the first page.
*           hints       -   Pointer to a download hints interface, receiving
* generated hints
* Return value:
*           PDF_DATA_ERROR: A common error is returned. It can't tell
*                           whehter data are availabe or not.
*           PDF_DATA_NOTAVAIL: Data are not yet available.
*           PDF_DATA_AVAIL: Data are available.
* Comments:
*           This function can be called only after FPDFAvail_GetDocument is
*           called. Applications should call this function whenever new data
*           arrived and process all the generated download hints if any, until
*           this function returns PDF_DATA_ERROR or PDF_DATA_AVAIL. Then
*           applications can perform page loading.
*/
DLLEXPORT int STDCALL FPDFAvail_IsPageAvail(FPDF_AVAIL avail,
                                            int page_index,
                                            FX_DOWNLOADHINTS *hints);

/**
* Function: FPDFAvail_ISFormAvail
*           Check whether Form data is ready for init, if not, get download
* hints.
*
* Parameters:
*           avail       -   Handle to document availability provider returned by
* FPDFAvail_Create
*           hints       -   Pointer to a download hints interface, receiving
* generated hints
* Return value:
*           PDF_FORM_ERROR    - A common eror, in general incorrect parameters,
*                               like 'hints' is nullptr.
*           PDF_FORM_NOTAVAIL - data not available
*           PDF_FORM_AVAIL    - data available
*           PDF_FORM_NOTEXIST - no form data
* Comments:
*           This function can be called only after FPDFAvail_GetDocument is
*           called.
*           The application should call this function whenever new data arrived,
* and process all the
*           generated download hints if any, until the function returns non-zero
* value. Then the
*           application can perform page loading. Recommend to call
* FPDFDOC_InitFormFillEnvironment
*           after the function returns non-zero value.
*/
DLLEXPORT int STDCALL FPDFAvail_IsFormAvail(FPDF_AVAIL avail,
                                            FX_DOWNLOADHINTS *hints);

/**
* Function: FPDFAvail_IsLinearized
*           To check whether a document is Linearized PDF file.
*
* Parameters:
*           avail       -   Handle to document availability provider returned by
* FPDFAvail_Create
* Return value:
*           PDF_LINEARIZED is a linearize file.
*           PDF_NOT_LINEARIZED is not a linearize file.
*           PDF_LINEARIZATION_UNKNOWN doesn't know whether the file is a
*linearize file.
*
* Comments:
*           It return PDF_LINEARIZED or PDF_NOT_LINEARIZED as soon as
*           we have first 1K data.  If the file's size less than 1K, it returns
*           PDF_LINEARIZATION_UNKNOWN because there is not enough information to
*           tell whether a PDF file is a linearized file or not.
*
*/
DLLEXPORT int STDCALL FPDFAvail_IsLinearized(FPDF_AVAIL avail);

#ifdef __cplusplus
}
#endif

#endif  // PUBLIC_FPDF_DATAAVAIL_H_
