#include "shader/util.glsl"

uniform BindlessSampler2D colorBuffer;
uniform BindlessSampler2D normalWaterBuffer;
uniform BindlessSampler2D matBuffer;
uniform vec2 pixelSize;
uniform mat4 invViewProjMatrix;
uniform vec3 eyePos;
uniform vec2 uCamParam;
uniform float udotl;

in vec2 vTexcoord;

out vec4 FragColor;

const vec4 OUTLINE_COLOR = vec4(0.0, 0.0, 0.0, 1.0);

vec4 GenColor(vec3 color, float depth) {
	vec4 res = vec4(1.0);
	#ifdef USE_CARTOON
		vec3 ndc = vec3(vTexcoord, depth) * 2.0 - 1.0;
		vec4 worldPos = invViewProjMatrix * vec4(ndc, 1.0); worldPos /= worldPos.w;
		float depthView = length(worldPos.xyz - eyePos);
		res.rgb = GenFogColor(-0.0000025, worldPos, depthView, udotl * 1.5, color);
	#else
		res.rgb = color;
	#endif
	return res;
}

void main() {
	vec4 color = texture(colorBuffer, vTexcoord);
	float depth = color.w;
	vec4 finalColor = vec4(color.rgb, 1.0);
	float flag = texture(matBuffer, vTexcoord).a;
	if (CompMat(flag, GrassFlag)) {
		FragColor = GenColor(finalColor.rgb, depth);
		return;
	}

	float separation = 1.2;
	vec2 off = pixelSize * separation;

	vec2 ld = vTexcoord + vec2(-off.x, -off.y);
	vec2 dd = vTexcoord + vec2(0.0,    -off.y);
	vec2 rd = vTexcoord + vec2(off.x,  -off.y);
	vec2 ll = vTexcoord + vec2(-off.x,    0.0);
	vec2 rr = vTexcoord + vec2(off.x,     0.0);
	vec2 lt = vTexcoord + vec2(-off.x,  off.y);
	vec2 tt = vTexcoord + vec2(0.0,     off.y);
	vec2 rt = vTexcoord + vec2(off.x,   off.y);
	vec2 ct = vTexcoord;
	
	/*
	float m0 = texture(matBuffer, ld).a;
	float m1 = texture(matBuffer, dd).a;
	float m2 = texture(matBuffer, rd).a;
	float m3 = texture(matBuffer, ll).a;
	float m4 = texture(matBuffer, rr).a;
	float m5 = texture(matBuffer, lt).a;
	float m6 = texture(matBuffer, tt).a;
	float m7 = texture(matBuffer, rt).a;

	if (CompMat(m0, GrassFlag) || CompMat(m1, GrassFlag) || CompMat(m2, GrassFlag) || CompMat(m3, GrassFlag) ||
		CompMat(m4, GrassFlag) || CompMat(m5, GrassFlag) || CompMat(m6, GrassFlag) || CompMat(m7, GrassFlag)) {
			FragColor = GenColor(finalColor.rgb, depth);
			return;
	}
	//*/

	vec4 c0 = texture(colorBuffer, ld);
	vec4 c1 = texture(colorBuffer, dd);
	vec4 c2 = texture(colorBuffer, rd);
	vec4 c3 = texture(colorBuffer, ll);
	vec4 c4 = texture(colorBuffer, rr);
	vec4 c5 = texture(colorBuffer, lt);
	vec4 c6 = texture(colorBuffer, tt);
	vec4 c7 = texture(colorBuffer, rt);

	vec4 depth1 = vec4(c0.w, c1.w, c2.w, c3.w);
	vec4 depth2 = vec4(c4.w, c5.w, c6.w, c7.w);

	vec4 lDepth1 = Linearize(uCamParam.x, uCamParam.y, depth1);
	vec4 lDepth2 = Linearize(uCamParam.x, uCamParam.y, depth2);
	float lDepth = Linearize(uCamParam.x, uCamParam.y, depth);
	
	vec4 dDepth1 = abs(lDepth1 - lDepth);
	vec4 dDepth2 = abs(lDepth2 - lDepth);
	
	vec4 minDDepth = max(min(dDepth1, dDepth2), 0.00001);
	vec4 maxDDepth = max(dDepth1, dDepth2);
	vec4 depthResults = step(minDDepth * 4000.0, maxDDepth);

	vec4 ldn = texture(normalWaterBuffer, ld) * 2.0 - 1.0;
	vec4 ddn = texture(normalWaterBuffer, dd) * 2.0 - 1.0;
	vec4 rdn = texture(normalWaterBuffer, rd) * 2.0 - 1.0;
	vec4 lln = texture(normalWaterBuffer, ll) * 2.0 - 1.0;
	vec4 rrn = texture(normalWaterBuffer, rr) * 2.0 - 1.0;
	vec4 ltn = texture(normalWaterBuffer, lt) * 2.0 - 1.0;
	vec4 ttn = texture(normalWaterBuffer, tt) * 2.0 - 1.0;
	vec4 rtn = texture(normalWaterBuffer, rt) * 2.0 - 1.0;
	vec4 ctn = texture(normalWaterBuffer, ct) * 2.0 - 1.0;

	vec3 normal = ctn.xyz;
	vec4 dNormal1 = vec4(dot(normal, ldn.xyz),
						 dot(normal, ddn.xyz),
						 dot(normal, rdn.xyz),
						 dot(normal, lln.xyz));
	vec4 dNormal2 = vec4(dot(normal, rrn.xyz),
						 dot(normal, ltn.xyz),
						 dot(normal, ttn.xyz),
						 dot(normal, rtn.xyz));
	vec4 dotDeltas = abs(dNormal1 - dNormal2);
    vec4 normalResults = step(0.5, dotDeltas);

	vec4 wfs1 = vec4(ldn.w, ddn.w, rdn.w, lln.w) * 0.5 + 0.5;
	vec4 wfs2 = vec4(rrn.w, ltn.w, ttn.w, rtn.w) * 0.5 + 0.5;
	float waterFactor = ctn.w * 0.5 + 0.5;
	
	vec4 dWater1 = vec4(waterFactor) - wfs1;
	vec4 dWater2 = vec4(waterFactor) - wfs2;
	vec4 waterResulats = step(0.4, abs(dWater1 + dWater2));

	vec4 results = max(normalResults, depthResults) * (1.0 - waterFactor);

	results = max(waterResulats, results);
	float edgeWeight = dot(results, vec4(1.0)) * 0.25;
	
	if(edgeWeight > 0.25) {
		#ifdef USE_CARTOON
			finalColor = OUTLINE_COLOR;
		#else
			vec3 sum = ZERO_VEC3;
			sum += c0.rgb + c1.rgb + c2.rgb + c3.rgb;
			sum += c4.rgb + c5.rgb + c6.rgb + c7.rgb;
			finalColor = vec4(mix(color.rgb, sum * 0.125, edgeWeight), 1.0);
		#endif
	} 
	
	FragColor = GenColor(finalColor.rgb, depth);
}