/* http://keithbriggs.info/software/LambertW.c
   Lambert W function.
   Was ~/C/LambertW.c written K M Briggs Keith dot Briggs at bt dot com 97 May 21.
   Revised KMB 97 Nov 20; 98 Feb 11, Nov 24, Dec 28; 99 Jan 13; 00 Feb 23; 01 Apr 09

   Computes Lambert W function, principal branch.
   See LambertW1.c for -1 branch.

   Returned value W(z) satisfies W(z)*exp(W(z))=z
   test data...
      W(1)= 0.5671432904097838730
      W(2)= 0.8526055020137254914
      W(20)=2.2050032780240599705
   To solve (a+b*R)*exp(-c*R)-d=0 for R, use
   R=-(b*W(-exp(-a*c/b)/b*d*c)+a*c)/b/c
*/

#include <math.h>
#include <stdio.h>
#include <assert.h>

double lambert_w(const double z) {
    int i;
    const double eps = 4.0e-16, em1 = 0.3678794411714423215955237701614608;
    double p, e, t, w;
    if (z < -em1 || isinf(z) || isnan(z)) {
        fprintf(stderr, "lambert_w: bad argument %g, exiting.\n", z); assert(0);
    }
    if (0.0 == z) return 0.0;
    if (z < -em1 + 1e-4) { // series near -em1 in sqrt(q)
        double q = z + em1, r = sqrt(q), q2 = q * q, q3 = q2 * q;
        return
            -1.0
            + 2.331643981597124203363536062168 * r
            - 1.812187885639363490240191647568 * q
            + 1.936631114492359755363277457668 * r * q
            - 2.353551201881614516821543561516 * q2
            + 3.066858901050631912893148922704 * r * q2
            - 4.175335600258177138854984177460 * q3
            + 5.858023729874774148815053846119 * r * q3
            - 8.401032217523977370984161688514 * q3 * q;  // error approx 1e-16
    }
    /* initial approx for iteration... */
    if (z < 1.0) { /* series near 0 */
        p = sqrt(2.0 * (2.7182818284590452353602874713526625 * z + 1.0));
        w = -1.0 + p * (1.0 + p * (-0.333333333333333333333 + p * 0.152777777777777777777777));
    }
    else
        w = log(z); /* asymptotic */
    if (z > 3.0) w -= log(w); /* useful? */
    for (i = 0; i < 10; i++) { /* Halley iteration */
        e = exp(w);
        t = w * e - z;
        p = w + 1.0;
        t /= e * p - 0.5 * (p + 1.0) * t / p;
        w -= t;
        if (fabs(t) < eps * (1.0 + fabs(w))) return w; /* rel-abs error */
    }
    /* should never get here */
    fprintf(stderr, "lambert_w: No convergence at z=%g, exiting.\n", z);
    assert(0);
}
