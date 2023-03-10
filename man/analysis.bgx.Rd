\name{analysis.bgx}
\alias{analysis.bgx}
\alias{plotExpressionDensity}
\alias{plotDEDensity}
\alias{plotDEHistogram}
\alias{rankByDE}
\alias{plotDiffRank}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{Analyse BGX output.}
\description{
  Functions for plotting expression densities, differential expression densities, histogram of proportion of differentially expressed genes, etc.
}
\usage{
  plotExpressionDensity(bgxOutput, gene=NULL, normalize=c("none","mean","loess"),...)
  plotDEDensity(bgxOutput, gene=NULL, conditions=c(1,2), normalize=c("none","mean","loess"), normgenes=c(1:length(bgxOutput[["geneNames"]])), ...)
  plotDEHistogram(bgxOutput, conditions=c(1,2), normalize=c("none","mean","loess"), normgenes=c(1:length(bgxOutput[["geneNames"]])), df=floor(1.8 * log10(length(bgxOutput[["geneNames"]]))))
  rankByDE(bgxOutput, conditions=c(1,2),normalize=c("none", "mean", "loess"), normgenes=c(1:length(bgxOutput[["geneNames"]])), absolute=TRUE)
  plotDiffRank(bgxOutput, conditions=c(1,2),normalize=c("none", "mean", "loess"), normgenes=c(1:length(bgxOutput[["geneNames"]])), ymax=NULL)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{bgxOutput}{A list obtained from running \code{\link{readOutput.bgx}} on a BGX output directory.}
  \item{gene}{Which gene to analyse. This can either be an index or a name.}
  \item{conditions}{Indices of conditions to compare.}
  \item{normalize}{"none": do not normalise posterior distributions of mu. "mean": normalise by scaling posterior distributions of mu for conditions > 1 to have the same mean as the posterior distribution of mu for condition 1. "loess": same as "mean" but use loess normalisation.}
  \item{normgenes}{Which genes to use for loess normalisation. By default, use all genes.}
  \item{df}{Residual degrees of freedom. Decrease to 6 if the histogram fit goes haywire.}
  \item{absolute}{Rank genes by absolute differential expression.}
  \item{ymax}{Specify upper limit of y axis.}
  \item{...}{Parameters to pass to density function (where applicable).}
}
\details{
  \code{plotExpressionDensity} plots gene expression distributions under various conditions for the specified gene.

  \code{plotDEDensity} plots the differential expression distribution between two conditions for a given gene.

  \code{plotDEHistogram} plots a histogram of differential expression between two conditions and estimates the number of up and down regulated differentially expressed genes.

  \code{rankByDE} ranks genes by differential expression and returns ordering and corresponding DE values in a matrix.

  \code{plotDiffRank} plots 2.5-97.5\% confidence intervals for ranked differential expression estimates.
}
\value{
  None, except \code{plotDERank}, which returns a matrix of genes ranked by differential expression.
}
\author{Ernest Turro}
\seealso{\code{\link{bgx}}, \code{\link{standalone.bgx}}, \code{\link{readOutput.bgx}}, \code{\link{plotExpressionDensity}}, \code{\link{plotDEDensity}}, \code{\link{plotDEHistogram}}}
\examples{
}
\keyword{IO}% at least one, from doc/KEYWORDS

