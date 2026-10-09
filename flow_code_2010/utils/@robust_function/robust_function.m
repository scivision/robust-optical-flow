classdef robust_function
%ROBUST_FUNCTION Robust penalty function.

% Copyright 2004-2007, Brown University, Providence, RI. USA
% Copyright 2007-2010 TU Darmstadt, Darmstadt, Germany.
%
% All commercial use of this software, whether direct or indirect, is
% strictly prohibited including, without limitation, incorporation into a
% commercial product, use in a commercial service, or production of other
% artifacts for commercial purposes.
%
% Permission to use, copy, modify, and distribute this software and its
% documentation for research purposes is hereby granted without fee,
% provided that the above copyright notice appears in all copies and that
% both that copyright notice and this permission notice appear in
% supporting documentation, and that the name of the author and Brown
% University not be used in advertising or publicity pertaining to
% distribution of the software without specific, written prior permission.
%
% For commercial uses contact the Technology Venture Office of Brown University.
%
% THE AUTHOR AND BROWN UNIVERSITY DISCLAIM ALL WARRANTIES WITH REGARD TO
% THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
% FITNESS FOR ANY PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR OR
% BROWN UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
% DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
% PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
% ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
% THIS SOFTWARE.

properties (SetAccess=private)
  type = @quadratic
end

properties
  param = 1
end

properties (Dependent)
  sigma
end

methods
  function this = robust_function(varargin)
    narginchk(0, 3)

    if nargin == 1 && isa(varargin{1}, 'robust_function')
      this = varargin{1};
      return
    end

    if nargin == 0
      return
    end

    name = varargin{1};
    types = {'geman_mcclure', 'huber', 'lorentzian', 'quadratic', ...
      'tukey', 'spline', 'mixture', 'gaussian', 'tdist', ...
      'tdist_unnorm', 'charbonnier', 'generalized_charbonnier'};
    if ~ischar(name) || ~any(strcmp(name, types))
      error('Invalid robust function type.')
    end

    switch name
      case 'geman_mcclure'
        this.type = @geman_mcclure;
      case 'huber'
        this.type = @huber;
      case 'lorentzian'
        this.type = @lorentzian;
      case 'quadratic'
        this.type = @quadratic;
      case 'tukey'
        this.type = @tukey;
      case 'spline'
        this.type = @spline;
      case 'mixture'
        this.type = @mixture;
      case 'gaussian'
        this.type = @gaussian;
      case 'tdist'
        this.type = @tdist;
      case 'tdist_unnorm'
        this.type = @tdist_unnorm;
      case 'charbonnier'
        this.type = @charbonnier;
      case 'generalized_charbonnier'
        this.type = @generalized_charbonnier;
    end
    if nargin == 1
      if strcmp(name, 'generalized_charbonnier')
        this.param = [1e-3, 1];
      end
    elseif nargin == 2 && ~strcmp(name, 'generalized_charbonnier')
      this.param = varargin{2};
    elseif nargin == 3 && strcmp(name, 'generalized_charbonnier')
      this.param = [varargin{2}, varargin{3}];
    else
      error('Invalid robust function type.')
    end
  end

  function value = get.sigma(this)
    value = this.param;
  end

  function this = set.sigma(this, value)
    this.param = value;
  end

  function disp(this)
    disp('Robust function type:')
    disp(this.type)
    disp('Parameter:')
    disp(this.param)
  end
end

methods
  y = deriv_over_x(this, x)
  y = deriv(this, x)
  y = evaluate(this, x)
end
end
