#include <Rcpp.h>
#include "../inst/include/RxODE.h"
#include <R.h>
#ifdef ENABLE_NLS
#include <libintl.h>
#define _(String) dgettext ("RxODE", String)
/* replace pkg as appropriate */
#else
#define _(String) (String)
#endif
using namespace Rcpp;

static inline CharacterVector cbindThetaOmegaNames(CharacterVector& inputN, CharacterVector& individualN) {
  CharacterVector retN(inputN.size() + individualN.size());
  for (int i = inputN.size(); i--; ) {
    retN[i] = inputN[i];
  }
  for (int i = individualN.size(); i--; ) {
    retN[inputN.size()+i] = individualN[i];
  }
  return retN;
}

List cbindThetaOmegaNM(NumericMatrix& inputParameters, List& individualParameters) {
  int tot = Rf_length(individualParameters[0]);
  int nstud = inputParameters.rows();
  List ret(inputParameters.ncol() + individualParameters.size()) ;
  CharacterVector inputN = as<CharacterVector>((as<List>(inputParameters.attr("dimnames")))[1]);
  CharacterVector individualN = individualParameters.attr("names");
  if (nstud == tot) {
    for (int i = inputN.size(); i--;) {
      NumericVector cur = inputParameters(_, i);
      ret[i] = cur;
    }
  } else if (tot %  nstud == 0) {
    int nsub = tot / nstud;
    for (int i = inputN.size(); i--; ) {
      NumericVector cur(tot);
      for (int j=0; j < nstud; j++) {
	std::fill_n(&cur[0] + j*nsub, nsub, inputParameters(j, i));
      }
      ret[i] = cur;
    }
  } else {
    stop("input parameter matrix does not match the number of studies (nStud) or total number of simulated subjects (nStud*nSub)");
  }
  for (int i = individualN.size(); i--; ) {
    ret[inputN.size() + i] = individualParameters[i];
  }
  ret.attr("names") = cbindThetaOmegaNames(inputN, individualN);
  ret.attr("class") = "data.frame";
  ret.attr("row.names") = IntegerVector::create(NA_INTEGER, -tot);
  return ret;
}

List cbindThetaOmegaL(List& inputParameters, List& individualParameters) {
  int tot = Rf_length(individualParameters[0]);
  int nstud = Rf_length(inputParameters[0]);
  List ret(inputParameters.size() + individualParameters.size()) ;
  CharacterVector inputN = inputParameters.attr("names");
  CharacterVector individualN = individualParameters.attr("names");
  if (nstud == tot) {
    for (int i = inputN.size(); i--;) {
      NumericVector cur = as<NumericVector>(inputParameters[i]);
      ret[i] = cur;
    }
  } else if (tot %  nstud == 0) {
    int nsub = tot / nstud;
    for (int i = inputN.size(); i--; ) {
      NumericVector cur(tot);
      NumericVector curIn = as<NumericVector>(inputParameters[i]);
      for (int j=0; j < nstud; j++) {
	std::fill_n(&cur[0] + j*nsub, nsub, curIn[j]);
      }
      ret[i] = cur;
    }
  } else {
    stop("input parameter data.frame does not match the number of studies (nStud) or total number of simulated subjects (nStud*nSub)");
  }
  for (int i = individualN.size(); i--; ) {
    ret[inputN.size() + i] = individualParameters[i];
  }
  ret.attr("names") = cbindThetaOmegaNames(inputN, individualN);
  ret.attr("class") = "data.frame";
  ret.attr("row.names") = IntegerVector::create(NA_INTEGER, -tot);
  return ret;
}

List cbindThetaOmega(RObject inputParameters, List& individualParameters) {
  if (Rf_isNull(inputParameters)) {
    return individualParameters;
  } else if (Rf_isMatrix(inputParameters)) {
    NumericMatrix ip = as<NumericMatrix>(inputParameters);
    return cbindThetaOmegaNM(ip, individualParameters);
  } else if (TYPEOF(inputParameters) == VECSXP) {
     List ip = as<List>(inputParameters);
     return cbindThetaOmegaL(ip, individualParameters);
  } else {
    stop(_("unexpected parameter object"));
  }
  return List::create();
}


extern "C" SEXP _rxCbindStudyIndividual(SEXP inputParameters, SEXP individualParameters) {
  RObject ip = as<RObject>(inputParameters);
  List ip2 = as<List>(individualParameters);
  SEXP ret = PROTECT(as<SEXP>(cbindThetaOmega(ip, ip2)));
  UNPROTECT(1);
  return ret;
}
