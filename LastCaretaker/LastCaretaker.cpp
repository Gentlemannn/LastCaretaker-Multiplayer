// LastCaretaker.cpp : Définit les fonctions exportées de la DLL.
//

#include "pch.h"
#include "framework.h"
#include "LastCaretaker.h"


// Il s'agit d'un exemple de variable exportée
LASTCARETAKER_API int nLastCaretaker=0;

// Il s'agit d'un exemple de fonction exportée.
LASTCARETAKER_API int fnLastCaretaker(void)
{
    return 0;
}

// Il s'agit du constructeur d'une classe qui a été exportée.
CLastCaretaker::CLastCaretaker()
{
    return;
}
