const canvas = document.getElementById('display');
const status = document.getElementById('status');
const consoleView = document.getElementById('console');
const mode = new URLSearchParams(location.search).get('mode') || 'framebuffer';

async function receiveConsole() {
  const response = await fetch('/console', { cache: 'no-store' });
  if (!response.ok) throw new Error(`HTTP ${response.status} from /console`);
  return response.text();
}

function textMode() {
  canvas.hidden = true;
  consoleView.hidden = false;
  const poll = () => receiveConsole().then((text) => {
	consoleView.textContent = text || 'Waiting for QEMU ttyS0 output…';
	consoleView.scrollTop = consoleView.scrollHeight;
	status.textContent = 'QEMU OS text mode — live ttyS0 output';
	setTimeout(poll, 100);
  }).catch((error) => {
	status.textContent = `QEMU console connection failed: ${error.message}`;
	setTimeout(poll, 1000);
  });
  poll();
}

async function receiveFrame() {
  const response = await fetch('/frame', { cache: 'no-store' });
  if (!response.ok) throw new Error(`HTTP ${response.status} from /frame`);
	const width = Number(response.headers.get('X-AyeDrive-Width'));
  const height = Number(response.headers.get('X-AyeDrive-Height'));
  if (!Number.isInteger(width) || !Number.isInteger(height) || width <= 0 || height <= 0) {
	throw new Error('missing or invalid negotiated framebuffer dimensions');
  }
  const bytes = new Uint8Array(await response.arrayBuffer());
	if (bytes.length !== width * height * 4) {
	throw new Error(`invalid framebuffer size ${bytes.length}, expected ${width * height * 4}`);
  }
	return { bytes, width, height };
}

function fallback() {
  const context = canvas.getContext('2d');
	let image;
  status.textContent = 'Canvas 2D active — waiting for C HAL frames from /frame';
  const draw = () => {
	receiveFrame().then((frame) => {
	  if (!image || image.width !== frame.width || image.height !== frame.height) {
		canvas.width = frame.width;
		canvas.height = frame.height;
		image = context.createImageData(frame.width, frame.height);
	  }
	  image.data.set(frame.bytes);
	  context.putImageData(image, 0, 0);
	  requestAnimationFrame(draw);
	}).catch((error) => {
	  status.textContent = `C HAL frame connection failed: ${error.message}`;
	  setTimeout(draw, 1000);
	});
  };
  draw();
}

async function webgpu() {
  const adapter = await navigator.gpu.requestAdapter();
  if (!adapter) throw new Error('No WebGPU adapter');
  const device = await adapter.requestDevice();
  const context = canvas.getContext('webgpu');
  const format = navigator.gpu.getPreferredCanvasFormat();
  context.configure({ device, format, alphaMode: 'opaque' });
  const sampler = device.createSampler({ magFilter: 'nearest', minFilter: 'nearest' });
  const module = device.createShaderModule({ code: `
	struct Out { @builtin(position) position: vec4f, @location(0) uv: vec2f }
	@vertex fn vs(@builtin(vertex_index) i: u32) -> Out { var p = array<vec2f,3>(vec2f(-1,-1),vec2f(3,-1),vec2f(-1,3)); var o:Out; o.position=vec4f(p[i],0,1); o.uv=(p[i]+1)/2; o.uv.y=1-o.uv.y; return o; }
	@group(0) @binding(0) var tex: texture_2d<f32>; @group(0) @binding(1) var smp: sampler;
	@fragment fn fs(i:Out) -> @location(0) vec4f { return textureSample(tex,smp,i.uv); }
  ` });
  const pipeline = device.createRenderPipeline({ layout: 'auto', vertex: { module, entryPoint: 'vs' }, fragment: { module, entryPoint: 'fs', targets: [{ format }] }, primitive: { topology: 'triangle-list' } });
	status.textContent = 'WebGPU active — waiting for C HAL frames from /frame';
  let texture;
  let bind;
	let textureWidth = 0;
  let textureHeight = 0;
  function draw() {
	receiveFrame().then((frame) => {
	  if (!texture || textureWidth !== frame.width || textureHeight !== frame.height) {
		canvas.width = frame.width;
		canvas.height = frame.height;
		texture?.destroy();
		texture = device.createTexture({ size: [frame.width, frame.height], format: 'rgba8unorm', usage: GPUTextureUsage.COPY_DST | GPUTextureUsage.TEXTURE_BINDING });
		textureWidth = frame.width;
		textureHeight = frame.height;
		bind = device.createBindGroup({ layout: pipeline.getBindGroupLayout(0), entries: [{ binding: 0, resource: texture.createView() }, { binding: 1, resource: sampler }] });
	  }
	  device.queue.writeTexture({ texture }, frame.bytes, { bytesPerRow: frame.width * 4 }, { width: frame.width, height: frame.height });
	  const encoder = device.createCommandEncoder();
	  const pass = encoder.beginRenderPass({ colorAttachments: [{ view: context.getCurrentTexture().createView(), clearValue: { r: 0, g: 0, b: 0, a: 1 }, loadOp: 'clear', storeOp: 'store' }] });
	  pass.setPipeline(pipeline); pass.setBindGroup(0, bind); pass.draw(3); pass.end();
	  device.queue.submit([encoder.finish()]);
	  requestAnimationFrame(draw);
	}).catch((error) => {
	  status.textContent = `C HAL frame connection failed: ${error.message}`;
	  setTimeout(draw, 1000);
	});
  }
  draw();
}

if (mode === 'text') textMode();
else if ('gpu' in navigator) webgpu().catch((error) => { status.textContent = `WebGPU unavailable: ${error.message}`; fallback(); });
else fallback();
