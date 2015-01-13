//
//  Lol Engine - Sandbox program
//
//  Copyright © 2005—2015 Sam Hocevar <sam@hocevar.net>
//
//  This program is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//

#if HAVE_CONFIG_H
#   include "config.h"
#endif

#include <lol/engine.h>

using namespace lol;

int main()
{
    bigint<16> i;
    bigint<16> x(2), y(12);
    auto z = x + y;
    printf("%d\n", (int)z);

    bigint<0> lol;
    auto w = z + lol;
    printf("%d\n", (int)w);
}

