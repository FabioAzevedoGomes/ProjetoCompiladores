#include <stdio.h>
#include <stdlib.h>

#include "error_report.h"

error_t *create_error(int type)
{
    error_t *error = (error_t *)malloc(sizeof(error_t));

    // Initialize error type and data count
    error->error_type = type;

    // Return pointer to created structure
    return error;
}
