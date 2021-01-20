<!--
---
output:
  md_document:
    variant: markdown_github
    toc: true
    toc_depth: 3
---
-->
<!-- README.md is generated from README.Rmd. Please edit that file -->
<!-- https://blog.r-hub.io/2019/12/03/readmes/ -->



# RxODE

<!-- badges: start -->
[![R build status](https://github.com/nlmixrdevelopment/RxODE/workflows/R-CMD-check/badge.svg)](https://github.com/nlmixrdevelopment/RxODE/actions)
[![codecov.io](https://codecov.io/github/nlmixrdevelopment/RxODE/coverage.svg)](https://codecov.io/github/nlmixrdevelopment/RxODE?branch=master)
[![CRAN version](http://www.r-pkg.org/badges/version/RxODE)](https://cran.r-project.org/package=RxODE)
[![CRAN total downloads](https://cranlogs.r-pkg.org/badges/grand-total/RxODE)](https://cran.r-project.org/package=RxODE)
[![CRAN total downloads](https://cranlogs.r-pkg.org/badges/RxODE)](https://cran.r-project.org/package=RxODE)
[![CodeFactor](https://www.codefactor.io/repository/github/nlmixrdevelopment/rxode/badge)](https://www.codefactor.io/repository/github/nlmixrdevelopment/rxode)
<!-- badges: end -->

## Overview

**RxODE** is an R package for solving and simulating from ode-based
models. These models are convert the RxODE mini-language to C and
create a compiled dll for fast solving. ODE solving using RxODE has a
few key parts:

 - `RxODE()` which creates the C code for fast ODE solving based on a
   [simple syntax](https://nlmixrdevelopment.github.io/RxODE/articles/RxODE-syntax.html) related to Leibnitz notation.
 - The event data, which can be:
   - a `NONMEM` or `deSolve` [compatible data frame](https://nlmixrdevelopment.github.io/RxODE/articles/RxODE-event-types.html), or
   - created with `et()` or `EventTable()` for [easy simulation of events](https://nlmixrdevelopment.github.io/RxODE/articles/RxODE-event-table.html)
   - The data frame can be augmented by adding
     [time-varying](https://nlmixrdevelopment.github.io/RxODE/articles/RxODE-covariates.html#time-varying-covariates)
     or adding [individual covariates](file:///home/matt/src/RxODE/docs/articles/RxODE-covariates.html#individual-covariates) (`iCov=` as needed)
 - `rxSolve()` which solves the system of equations using initial
   conditions and parameters to make predictions
   - With multiple subject data, [this may be
     parallelized](https://nlmixrdevelopment.github.io/RxODE/articles/RxODE-speed.html).
   - With single subject the [output data frame is adaptive](https://nlmixrdevelopment.github.io/RxODE/articles/RxODE-data-frame.html)
   - Covariances and other metrics of uncertanty can be used to
     [simulate while solving](https://nlmixrdevelopment.github.io/RxODE/articles/RxODE-sim-var.html)

## Installation



You can install the released version of RxODE from
[CRAN](https://CRAN.R-project.org) with:

``` r
install.packages("RxODE")
```

You can install the development version of RxODE with

```r
devtools::install_github("nlmixrdevelopment/RxODE")
```

To build models with RxODE, you need a working c compiler.  To use parallel threaded
solving in RxODE, this c compiler needs to support open-mp.

You can check to see if R has working c compiler you can check with:

```r
## install.packages("pkgbuild")
pkgbuild::has_build_tools(debug = TRUE)
```

If you do not have the toolchain, you can set it up as described by
the platform information below:

### Windows

In windows you may simply use installr to install rtools:

```r
install.packages("installr")
library(installr)
install.rtools()
```

Alternatively you can
[download](https://cran.r-project.org/bin/windows/Rtools/) and install
rtools directly.

### Mac OSX

To get the most speed you need OpenMP enabled and compile RxODE
against that binary.  Here is some discussion about this:

https://ryanhomer.github.io/posts/build-openmp-macos-catalina-complete

Briefly, I would install R from CRAN and then install `RxODE` (which
installs the additional dependencies).  Then for best speed, install
homebrew to compile `OpenMP` dependencies so they can run in
multi-threaded mode.  You could do this with Mac's Xcode, but it often
requires the very latest MacOS version; Depending on when you do this
it can possibly break R packages, so it is no longer recommended.

Once [homebrew is installed](https://brew.sh/), use it to install
OpenMP enabled compilers:

```sh
brew install llvm libomp
```

And the gfortran compiler needed for `RxODE`:

```sh
brew install gcc
```

Some of the functions of `RxODE` and `nlmixr` rely on extra components
being installed, to be safe I would install the following:

```sh
brew install cairo # Installs some items needed to optionally compile componets for ggplot2
brew install --cask xquartz # Installs components for huxtable/officer
```

Then edit the file `~/.R/Makevars` to use the OpenMP.  In R/Rstudio
you can edit this file by:

```
dir.create("~/.R") # may error if exists
file.edit("~/.R/Makevars")
```

```
# macOS Makevars configuration for LLVM/GCC
# for OpenMP support
#
# For installation details, see
# http://ryanhomer.github.io/posts/build-openmp-macos-catalina-complete
#
# Some sources used as reference:
# https://github.com/Rdatatable/data.table/wiki/Installation
# https://asieira.github.io/using-openmp-with-r-packages-in-os-x.html
# https://thecoatlessprofessor.com/programming/openmp-in-r-on-os-x/
# https://bit.ly/3d16TuW
# https://www.kthohr.com/r-mac-source.html

XCBASE:=$(shell xcrun --show-sdk-path)
LLVMBASE:=$(shell brew --prefix llvm)
GCCBASE:=$(shell brew --prefix gcc)
GETTEXT:=$(shell brew --prefix gettext)

CC=$(LLVMBASE)/bin/clang -fopenmp
CXX=$(LLVMBASE)/bin/clang++ -fopenmp
CXX11=$(LLVMBASE)/bin/clang++ -fopenmp
CXX14=$(LLVMBASE)/bin/clang++ -fopenmp
CXX17=$(LLVMBASE)/bin/clang++ -fopenmp
CXX1X=$(LLVMBASE)/bin/clang++ -fopenmp

CPPFLAGS=-isystem "$(LLVMBASE)/include" -isysroot "$(XCBASE)"
LDFLAGS=-L"$(LLVMBASE)/lib" -L"$(GETTEXT)/lib" --sysroot="$(XCBASE)"

FC=$(GCCBASE)/bin/gfortran
F77=$(GCCBASE)/bin/gfortran
# This # matches the gfortran version, you can see the version by the command
# `gfortran --version`
FLIBS=-L$(GCCBASE)/lib/gcc/10/ -lm
```

This works to install `data.table`, `RxODE` and `nlmixr` with `OpenMP`
in R 4.0+ support.  However, some R packages that use `autoconf` will
not work with this `Makevars`; So I would use:

```r
install.packages("data.table", type="source")
install.packages("RxODE", type="source") # or remotes::install_github("nlmixrdevelopment/RxODE")
install.packages("nlmixr", type="source") # or remotes::instal_github("nlmixrdevelopment/nlmixr")
```

To be safe, do not install packages that require compiled binaries
with this approach.  Once complete you can remove the `-fopenmp` flag in the `Makevars`
and the compiler will work without enabling `OpenMP`:

```
# macOS Makevars configuration for LLVM/GCC
# for OpenMP support
#
# For installation details, see
# http://ryanhomer.github.io/posts/build-openmp-macos-catalina-complete
#
# Some sources used as reference:
# https://github.com/Rdatatable/data.table/wiki/Installation
# https://asieira.github.io/using-openmp-with-r-packages-in-os-x.html
# https://thecoatlessprofessor.com/programming/openmp-in-r-on-os-x/
# https://bit.ly/3d16TuW
# https://www.kthohr.com/r-mac-source.html

XCBASE:=$(shell xcrun --show-sdk-path)
LLVMBASE:=$(shell brew --prefix llvm)
GCCBASE:=$(shell brew --prefix gcc)
GETTEXT:=$(shell brew --prefix gettext)

CC=$(LLVMBASE)/bin/clang 
CXX=$(LLVMBASE)/bin/clang++ 
CXX11=$(LLVMBASE)/bin/clang++ 
CXX14=$(LLVMBASE)/bin/clang++ 
CXX17=$(LLVMBASE)/bin/clang++
CXX1X=$(LLVMBASE)/bin/clang++

CPPFLAGS=-isystem "$(LLVMBASE)/include" -isysroot "$(XCBASE)"
LDFLAGS=-L"$(LLVMBASE)/lib" -L"$(GETTEXT)/lib" --sysroot="$(XCBASE)"

FC=$(GCCBASE)/bin/gfortran
F77=$(GCCBASE)/bin/gfortran
# This # matches the gfortran version, you can see the version by the command
# `gfortran --version`
FLIBS=-L$(GCCBASE)/lib/gcc/10/ -lm
```

### Linux

To install on linux make sure you install `gcc` (with openmp support)
and `gfortran` using your distribution's package manager.

## Development Version

Since the development version of RxODE uses StanHeaders, you will need
to make sure your compiler is setup to support C++14, as described in
the [rstan setup page](https://github.com/stan-dev/rstan/wiki/RStan-Getting-Started#configuration-of-the-c-toolchain)

Once the C++ toolchain is setup appropriately, you can install the
development version from
[GitHub](https://github.com/nlmixrdevelopment/RxODE) with:

``` r
# install.packages("devtools")
devtools::install_github("nlmixrdevelopment/RxODE")
```

# Illustrated Example


The model equations can be specified through a text string, a model
file or an R expression. Both differential and algebraic equations are
permitted. Differential equations are specified by `d/dt(var_name) = `. Each
equation can be separated by a semicolon.

To load `RxODE` package and compile the model: 


```r
library(RxODE)
#> RxODE 1.0.2 using 4 threads (see ?getRxThreads)
library(units)
#> udunits system database from /usr/share/xml/udunits

mod1 <-RxODE({
    C2 = centr/V2;
    C3 = peri/V3;
    d/dt(depot) =-KA*depot;
    d/dt(centr) = KA*depot - CL*C2 - Q*C2 + Q*C3;
    d/dt(peri)  =                    Q*C2 - Q*C3;
    d/dt(eff)  = Kin - Kout*(1-C2/(EC50+C2))*eff;
})
#> 
#> qs v0.23.5.
```

## Specify ODE parameters and initial conditions

Model parameters can be defined as named vectors. Names of parameters in
the vector must be a superset of parameters in the ODE model, and the
order of parameters within the vector is not important. 


```r
theta <- 
   c(KA=2.94E-01, CL=1.86E+01, V2=4.02E+01, # central 
     Q=1.05E+01,  V3=2.97E+02,              # peripheral
     Kin=1, Kout=1, EC50=200)               # effects
```

Initial conditions (ICs) can be defined through a vector as well.  If the
elements are not specified, the initial condition for the compartment
is assumed to be zero.



```r
inits <- c(eff=1);
```

If you want to specify the initial conditions in the model you can add:

```
eff(0) = 1
```

## Specify Dosing and sampling in RxODE

`RxODE` provides a simple and very flexible way to specify dosing and
sampling through functions that generate an event table. First, an
empty event table is generated through the "eventTable()" function:


```r
ev <- eventTable(amount.units='mg', time.units='hours')
```

Next, use the `add.dosing()` and `add.sampling()` functions of the
`EventTable` object to specify the dosing (amounts, frequency and/or
times, etc.) and observation times at which to sample the state of the
system.  These functions can be called multiple times to specify more
complex dosing or sampling regiments.  Here, these functions are used
to specify 10mg BID dosing for 5 days, followed by 20mg QD dosing for
5 days:


```r
ev$add.dosing(dose=10000, nbr.doses=10, dosing.interval=12)
ev$add.dosing(dose=20000, nbr.doses=5, start.time=120,
              dosing.interval=24)
ev$add.sampling(0:240)
```

If you wish you can also do this with the `mattigr` pipe operator `%>%`


```r
ev <- eventTable(amount.units="mg", time.units="hours") %>%
  add.dosing(dose=10000, nbr.doses=10, dosing.interval=12) %>%
  add.dosing(dose=20000, nbr.doses=5, start.time=120,
             dosing.interval=24) %>%
  add.sampling(0:240)
```

The functions `get.dosing()` and `get.sampling()` can be used to
retrieve information from the event table.


```r
head(ev$get.dosing())
#>   id low time high       cmt   amt rate ii addl evid ss dur
#> 1  1  NA    0   NA (default) 10000    0 12    9    1  0   0
#> 2  1  NA  120   NA (default) 20000    0 24    4    1  0   0
```


```r
head(ev$get.sampling())
#>   id low time high   cmt amt rate ii addl evid ss dur
#> 1  1  NA    0   NA (obs)  NA   NA NA   NA    0 NA  NA
#> 2  1  NA    1   NA (obs)  NA   NA NA   NA    0 NA  NA
#> 3  1  NA    2   NA (obs)  NA   NA NA   NA    0 NA  NA
#> 4  1  NA    3   NA (obs)  NA   NA NA   NA    0 NA  NA
#> 5  1  NA    4   NA (obs)  NA   NA NA   NA    0 NA  NA
#> 6  1  NA    5   NA (obs)  NA   NA NA   NA    0 NA  NA
```

You may notice that these are similar to NONMEM event tables; If you
are more familiar with NONMEM data and events you could use them
directly with the event table function `et`


```r
ev  <- et(amountUnits="mg", timeUnits="hours") %>%
  et(amt=10000, addl=9,ii=12,cmt="depot") %>%
  et(time=120, amt=2000, addl=4, ii=14, cmt="depot") %>%
  et(0:240) # Add sampling 
```

You can see from the above code, you can dose to the compartment named
in the RxODE model.  This slight deviation from NONMEM can reduce the
need for compartment renumbering.

These events can also be combined and expanded (to multi-subject
events and complex regimens) with `rbind`, `c`, `seq`, and `rep`. For
more information about creating complex dosing regimens using RxODE
see the [RxODE events
vignette](https://nlmixrdevelopment.github.io/RxODE.doc/articles/RxODE-events.html).


## Solving ODEs

The ODE can now be solved by calling the model object's `run` or `solve`
function. Simulation results for all variables in the model are stored
in the output matrix x. 


```r
x <- mod1$solve(theta, ev, inits);
knitr::kable(head(x))
```



| time|       C2|        C3|     depot|    centr|      peri|      eff|
|----:|--------:|---------:|---------:|--------:|---------:|--------:|
|    0|  0.00000| 0.0000000| 10000.000|    0.000|    0.0000| 1.000000|
|    1| 44.37555| 0.9198298|  7452.765| 1783.897|  273.1895| 1.084664|
|    2| 54.88296| 2.6729825|  5554.370| 2206.295|  793.8758| 1.180825|
|    3| 51.90343| 4.4564927|  4139.542| 2086.518| 1323.5783| 1.228914|
|    4| 44.49738| 5.9807076|  3085.103| 1788.795| 1776.2702| 1.234610|
|    5| 36.48434| 7.1774981|  2299.255| 1466.670| 2131.7169| 1.214742|

You can also solve this and create a RxODE data frame:



```r
x <- mod1 %>% rxSolve(theta, ev, inits);
x
#> ▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂ Solved RxODE object ▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂
#> ── Parameters (x$params): ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
#>      V2      V3      KA      CL       Q     Kin    Kout    EC50 
#>  40.200 297.000   0.294  18.600  10.500   1.000   1.000 200.000 
#> ── Initial Conditions (x$inits): ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
#> depot centr  peri   eff 
#>     0     0     0     1 
#> ── First part of data (object): ───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
#> # A tibble: 241 x 7
#>    time    C2    C3  depot centr  peri   eff
#>     [h] <dbl> <dbl>  <dbl> <dbl> <dbl> <dbl>
#> 1     0   0   0     10000     0     0   1   
#> 2     1  44.4 0.920  7453. 1784.  273.  1.08
#> 3     2  54.9 2.67   5554. 2206.  794.  1.18
#> 4     3  51.9 4.46   4140. 2087. 1324.  1.23
#> 5     4  44.5 5.98   3085. 1789. 1776.  1.23
#> 6     5  36.5 7.18   2299. 1467. 2132.  1.21
#> # … with 235 more rows
#> ▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂▂
```

This returns a modified data frame.  You can see the compartment
values in the plot below:


```r
library(ggplot2)
plot(x,C2) + ylab("Central Concentration")
```

<img src="man/figures/README-intro-central-1.png" title="plot of chunk intro-central" alt="plot of chunk intro-central" width="100%" />

Or, 


```r
plot(x,eff)  + ylab("Effect")
```

<img src="man/figures/README-intro-effect-1.png" title="plot of chunk intro-effect" alt="plot of chunk intro-effect" width="100%" />

Note that the labels are automatically labeled with the units from the
initial event table. RxODE extracts `units` to label the plot (if they
are present).

# Related R Packages

## ODE solving

This is a brief comparison of pharmacometric ODE solving R packages to
`RxODE`.

There are several [R packages for differential
equations](https://cran.r-project.org/web/views/DifferentialEquations.html).
The most popular is
[deSolve](https://cran.r-project.org/package=deSolve).

However for pharmacometrics-specific ODE solving, there are only 2
packages other than [RxODE](https://CRAN.R-project.org/package=RxODE)
released on CRAN.  Each uses compiled code to have faster ODE solving.

- [mrgsolve](https://CRAN.R-project.org/package=mrgsolve), which uses
  C++ lsoda solver to solve ODE systems.  The user is
  required to write hybrid R/C++ code to create a mrgsolve
  model which is translated to C++ for solving.

  In contrast, `RxODE` has a R-like mini-language that is parsed into
  C code that solves the ODE system.

  Unlike `RxODE`, `mrgsolve` does not currently support symbolic
  manipulation of ODE systems, like automatic Jacobian calculation or
  forward sensitivity calculation (`RxODE` currently supports this and
  this is the basis of
  [nlmixr](https://cran.r-project.org/package=nlmixr)'s FOCEi
  algorithm)

- [dMod](https://cran.r-project.org/package=dMod), which uses a unique
  syntax to create "reactions".  These reactions create the underlying
  ODEs and then created c code for a compiled deSolve model.

  In contrast `RxODE` defines ODE systems at a lower level.  `RxODE`'s
  parsing of the mini-language comes from C, whereas `dMod`'s parsing
  comes from R.

  Like `RxODE`, `dMod` supports symbolic manipulation of ODE systems
  and calculates forward sensitivities and adjoint sensitivities of
  systems.

  Unlike `RxODE`, `dMod` is not thread-safe since `deSolve` is not yet
  thread-safe.

And there is one package that is not released on CRAN:

- [PKPDsim](https://github.com/InsightRX/PKPDsim) which defines models
  in an R-like syntax and converts the system to compiled code.

  Like `mrgsolve`, `PKPDsim` does not currently support symbolic
  manipulation of ODE systems.

  `PKPDsim` is not thread-safe.

The open pharmacometrics open source community is fairly friendly, and
the RxODE maintainers has had positive interactions with all of the
ODE-solving pharmacometric projects listed.

## PK Solved systems

RxODE supports 1-3 compartment models with gradients (using stan
math's auto-differentiation).  This currently uses the same
equations as PKADVAN to allow time-varying covariates.

RxODE can mix ODEs and solved systems.

### The following packages for solved PK systems are on CRAN

 - [mrgsolve](https://CRAN.R-project.org/package=mrgsolve) currently
   has 1-2 compartment (poly-exponential models) models built-in.  The
   solved systems and ODEs cannot currently be mixed.
 - [pmxTools](https://github.com/kestrel99/pmxTools) currently have
   1-3 compartment (super-positioning) models built-in. This is a
   R-only implementation.
 - [PKPDmodels](https://cran.r-project.org/web/packages/PKPDmodels/index.html)
   has a one-compartment model with gradients.

### Non-CRAN libraries:

 - [PKADVAN](https://github.com/abuhelwa/PKADVAN_Rpackage) Provides
   1-3 compartment models using non-superpositioning.  This allows
   time-varying covariates.
