//
// Created by novot on 10/25/2020.
//

#ifndef IFJ2020_ERROR_H
#define IFJ2020_ERROR_H


#define ERR_LEX 1
#define ERR_SYN 2
#define ERR_SEM_UNDEF 3
#define ERR_SEM_DATATYPE 4
#define ERR_SEM_EXCOMPAT 5
#define ERR_SEM_RETURN 6
#define ERR_SEM_OTHER 7
#define ERR_SEM_ZERODIV 9
#define ERR_INTERN 99

void error_call(int err);

#endif //IFJ2020_ERROR_H
