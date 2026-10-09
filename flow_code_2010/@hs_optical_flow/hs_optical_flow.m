classdef hs_optical_flow
    %HS_OPTICAL_FLOW Optical flow computation with Horn & Schunck method.
    %   HS_OPTICAL_FLOW() creates an object with default parameters.
    %   HS_OPTICAL_FLOW(IMAGES) creates an object with image sequence IMAGES.
    %   HS_OPTICAL_FLOW(OTHER) copies another HS optical flow object.
    %
    % Copyright 2007-2010, Brown University, Providence, RI. USA
    %
    % All commercial use of this software, whether direct or indirect, is
    % strictly prohibited including, without limitation, incorporation into
    % a commercial product, use in a commercial service, or production of
    % other artifacts for commercial purposes.
    %
    % Permission to use, copy, modify, and distribute this software and its
    % documentation for research purposes is hereby granted without fee,
    % provided that the above copyright notice appears in all copies and
    % that both that copyright notice and this permission notice appear in
    % supporting documentation, and that the name of the author and Brown
    % University not be used in advertising or publicity pertaining to
    % distribution of the software without specific, written prior
    % permission.
    %
    % For commercial uses contact the Technology Venture Office of Brown
    % University.
    %
    % THE AUTHOR AND BROWN UNIVERSITY DISCLAIM ALL WARRANTIES WITH REGARD
    % TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
    % AND FITNESS FOR ANY PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR
    % OR BROWN UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
    % CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
    % OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
    % OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
    % USE OR PERFORMANCE OF THIS SOFTWARE.

    properties
        images = []
        lambda = 80
        lambda_q = 80
        gnc_iters = 1
        pyramid_levels = 4
        pyramid_spacing = 2
        max_warping_iters = 10
        max_iters = 10
        median_filter_size = []
        texture = false
        solver = 'backslash'
        sor_max_iters = 1e4
        interpolation_method = 'cubic'
        deriv_filter = [1 -8 0 8 -1] / 12
        display = false
        limit_update = true
        sigmaD2 = 1
        sigmaS2 = 1
        sigmaP = 1
        weightP = 1
        mf_iter = 1
        spatial_filters = {[1 -1], [1; -1]}
        rho_spatial_u = {}
        rho_spatial_v = {}
        rho_data = []
        color_images = []
        image_filter = []
    end

    methods
        function this = hs_optical_flow(varargin)
            narginchk(0, 1)

            if ~isempty(varargin) && isa(varargin{1}, 'hs_optical_flow')
                this = varargin{1};
            else
                for i = 1:length(this.spatial_filters)
                    this.rho_spatial_u{i} = robust_function('quadratic', 1);
                    this.rho_spatial_v{i} = robust_function('quadratic', 1);
                end
                this.rho_data = robust_function('quadratic', 1);
                if ~isempty(varargin)
                    this.images = varargin{1};
                end
            end
        end

        function disp(this)
            fprintf('Image sequence (size):          %s\n', ...
                mat2str(size(this.images)));
            fprintf('Spatial term weighting:         %s\n', ...
                mat2str(this.lambda));
            fprintf('Linear solver:                  %s\n', ...
                this.solver);
            fprintf('# of pyramid levels:            %d\n', ...
                this.pyramid_levels);
            fprintf('Spacing of pyramid levels:      %d\n', ...
                this.pyramid_spacing);
            fprintf('Max. # of iterations:           %d\n', ...
                this.max_warping_iters);
            fprintf('Limit flow updates              %d\n', ...
                this.limit_update);
        end

        uv = compute_flow(this, init, gt)
        uv = compute_flow_base(this, uv)
        duv = compute_flow_increment_cgd(this, uv)
        L = evaluate_log_posterior(this, uv)
        guv = evaluate_log_posterior_grad(this, uv)
        [A, b, params, iterative] = flow_operator(this, uv)
        this = parse_input_parameter(this, param)
    end
end
