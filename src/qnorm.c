/*
 *  This file is part of BGX, the Bayesian Gene eXpression program.
 *  Copyright (c) 2003-2004  Graeme Ambler <graeme@ambler.me.uk>
 *
 *  BGX is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License, version 2, as
 *  published by the Free Software Foundation.
 *
 *  BGX is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <math.h>
#include "qnorm.h"

// A really accurate qnorm routine from "The Percentage Points of the Normal 
// Distribution" by Michael J. Wichura, U of Chicago.  Appeared as algorithm 
// AS241 in Applied Statistics 37, pp. 477--484.
double qnorm(double p)
{
    const double split1=0.425;
    const double split2=5;
    const double const1=0.180625;
    const double const2=1.6;

    const double A0 = 3.3871328727963666080E0;
    const double A1 = 1.3314166789178437745E2;
    const double A2 = 1.9715909503065514427E3;
    const double A3 = 1.3731693765509461125E4;
    const double A4 = 4.5921953931549871457E4;
    const double A5 = 6.7265770927008700853E4;
    const double A6 = 3.3430575583588128105E4;
    const double A7 = 2.5090809287301226727E3;

    const double B1 = 4.2313330701600911252E1;
    const double B2 = 6.8718700749205790830E2;
    const double B3 = 5.3941960214247511077E3;
    const double B4 = 2.1213794301586595867E4;
    const double B5 = 3.9307895800092710610E4;
    const double B6 = 2.8729085735721942674E4;
    const double B7 = 5.2264952788528545610E3;

// Hash sum AB       55.88319 28806 14901 4439

    const double C0 = 1.42343711074968357734E0;
    const double C1 = 4.63033784615654529590E0;
    const double C2 = 5.76949722146069140550E0;
    const double C3 = 3.64784832476320460504E0;
    const double C4 = 1.27045825245236838258E0;
    const double C5 = 2.41780725177450611770E-1;
    const double C6 = 2.27238449892691845833E-2;
    const double C7 = 7.74545014278341407640E-4;

    const double D1 = 2.05319162663775882187E0;
    const double D2 = 1.67638483018380384940E0;
    const double D3 = 6.89767334985100004550E-1;
    const double D4 = 1.48103976427480074590E-1;
    const double D5 = 1.51986665636164571966E-2;
    const double D6 = 5.47593808499534494600E-4;
    const double D7 = 1.05075007164441684324E-9;

// Hash sum CD       49.33206 50330 16102 89036

    const double E0 = 6.65790464350110377720E0;
    const double E1 = 5.46378491116411436990E0;
    const double E2 = 1.78482653991729133580E0;
    const double E3 = 2.96560571828504891230E-1;
    const double E4 = 2.65321895265761230930E-2;
    const double E5 = 1.24266094738807843860E-3;
    const double E6 = 2.71155556874348787815E-4;
    const double E7 = 2.01033439929228813265E-7;

    const double F1 = 5.99832206555887937690E-1;
    const double F2 = 1.36929880922735805310E-1;
    const double F3 = 1.48753612908506148525E-2;
    const double F4 = 7.86869131145613259100E-4;
    const double F5 = 1.84631831751005468180E-5;
    const double F6 = 1.42151175831644588870E-7;
    const double F7 = 2.04426310338993978564E-15;

// Hash sum EF       47.52583 31754 92896 71639

    double q = p-0.5;
    double r;
    /*printf("Point1\n");*/
    if(p==0){ /*printf("Point2\n");*/ return -HUGE_VAL; }
    if(p==1){ /*printf("Point3\n");*/ return HUGE_VAL; }
    if(fabs(q)<=split1){
	r=const1-q*q;
	return q*( (((((((A7*r+A6)*r+A5)*r+A4)*r+A3)*r+A2)*r+A1)*r+A0) / 
		   (((((((B7*r+B6)*r+B5)*r+B4)*r+B3)*r+B2)*r+B1)*r+1) );
    }
    if(q<0)
	r=p;
    else
	r=1-p;
    if(r<=0)
	return NAN; // ERROR!!!!!  THIS CAN NEVER HAPPEN!!!
    r=sqrt(-log(r));
    if(r<=split2){
	r-=const2;
	if(q<0) return - (((((((C7*r+C6)*r+C5)*r+C4)*r+C3)*r+C2)*r+C1)*r+C0) /
		    (((((((D7*r+D6)*r+D5)*r+D4)*r+D3)*r+D2)*r+D1)*r+1);
	else return (((((((C7*r+C6)*r+C5)*r+C4)*r+C3)*r+C2)*r+C1)*r+C0) / 
		 (((((((D7*r+D6)*r+D5)*r+D4)*r+D3)*r+D2)*r+D1)*r+1);
    }
    r-=split2;
    if(q<0) return - (((((((E7*r+E6)*r+E5)*r+E4)*r+E3)*r+E2)*r+E1)*r+E0) /
		(((((((F7*r+F6)*r+F5)*r+F4)*r+F3)*r+F2)*r+F1)*r+1);
    else return (((((((E7*r+E6)*r+E5)*r+E4)*r+E3)*r+E2)*r+E1)*r+E0) / 
	     (((((((F7*r+F6)*r+F5)*r+F4)*r+F3)*r+F2)*r+F1)*r+1);
}

// A slow and dirty qnorm routine I found on the web somewhere, accurate 
// to within 1 part in 2000 in the range 0.00001 to 0.99999 (and probably 
// outside that range too, but I've only tested it on [0.00001,0.99999]).
// Its only merits are that it can be written in very few lines of code 
// and that it only requires a single temporary variable.
double qnorm2(double p)
{
  double z = (p>0.5) ? sqrt(-2.0*log(1-p)) : sqrt(-2.0*log(p));
  z-=((0.010328*z+0.802853)*z+2.515517) / 
    (((0.001308*z+0.189269)*z+1.43278)*z+1);
  if (p<0.5)
    return -z; 
  return z;
}
