const fs = require('fs');

const glbPath = 'data/resources/3dModels/human_model_nathan.glb';
const cleanSource = `${glbPath}.bak-handnodes`;
const backupPath = `${glbPath}.bak-before-walk-stop-finger-sequence`;

function align4(n) { return (n + 3) & ~3; }

function readGlb(filePath) {
  const buffer = fs.readFileSync(filePath);
  const jsonLength = buffer.readUInt32LE(12);
  const jsonStart = 20;
  const json = JSON.parse(buffer.slice(jsonStart, jsonStart + jsonLength).toString('utf8'));
  const binHeader = jsonStart + align4(jsonLength);
  const binLength = buffer.readUInt32LE(binHeader);
  const bin = buffer.slice(binHeader + 8, binHeader + 8 + binLength);
  return { json, bin };
}

function writeGlb(filePath, json, bin) {
  const jsonBuffer = Buffer.from(JSON.stringify(json), 'utf8');
  const jsonChunk = Buffer.alloc(align4(jsonBuffer.length), 0x20);
  jsonBuffer.copy(jsonChunk);
  const binChunk = Buffer.alloc(align4(bin.length));
  bin.copy(binChunk);
  const totalLength = 12 + 8 + jsonChunk.length + 8 + binChunk.length;
  const out = Buffer.alloc(totalLength);
  out.write('glTF', 0, 'ascii');
  out.writeUInt32LE(2, 4);
  out.writeUInt32LE(totalLength, 8);
  out.writeUInt32LE(jsonChunk.length, 12);
  out.writeUInt32LE(0x4e4f534a, 16);
  jsonChunk.copy(out, 20);
  const binHeader = 20 + jsonChunk.length;
  out.writeUInt32LE(binChunk.length, binHeader);
  out.writeUInt32LE(0x004e4942, binHeader + 4);
  binChunk.copy(out, binHeader + 8);
  fs.writeFileSync(filePath, out);
}

function normalize(q) {
  const len = Math.hypot(q[0], q[1], q[2], q[3]) || 1;
  return q.map(v => v / len);
}

function qz(rad) { return [0, 0, Math.sin(rad / 2), Math.cos(rad / 2)]; }
function qy(rad) { return [0, Math.sin(rad / 2), 0, Math.cos(rad / 2)]; }
function qx(rad) { return [Math.sin(rad / 2), 0, 0, Math.cos(rad / 2)]; }
function qmul(a, b) {
  return normalize([
    a[3] * b[0] + a[0] * b[3] + a[1] * b[2] - a[2] * b[1],
    a[3] * b[1] - a[0] * b[2] + a[1] * b[3] + a[2] * b[0],
    a[3] * b[2] + a[0] * b[1] - a[1] * b[0] + a[2] * b[3],
    a[3] * b[3] - a[0] * b[0] - a[1] * b[1] - a[2] * b[2],
  ]);
}

function nodeIndex(json, name) {
  const index = json.nodes.findIndex(n => n.name === name);
  if (index < 0) throw new Error(`Missing node ${name}`);
  return index;
}

function appendView(json, parts, data) {
  const current = parts.reduce((sum, p) => sum + p.length, 0);
  const offset = align4(current);
  if (offset > current) parts.push(Buffer.alloc(offset - current));
  parts.push(data);
  json.bufferViews.push({ buffer: 0, byteOffset: offset, byteLength: data.length });
  return json.bufferViews.length - 1;
}

function appendAccessor(json, parts, values, type, min, max) {
  const data = Buffer.alloc(values.length * 4);
  values.forEach((v, i) => data.writeFloatLE(v, i * 4));
  const bufferView = appendView(json, parts, data);
  const components = { SCALAR: 1, VEC3: 3, VEC4: 4 }[type];
  const accessor = { bufferView, byteOffset: 0, componentType: 5126, count: values.length / components, type };
  if (min) accessor.min = min;
  if (max) accessor.max = max;
  json.accessors.push(accessor);
  return json.accessors.length - 1;
}

function main() {
  if (!fs.existsSync(cleanSource)) throw new Error(`Missing clean source: ${cleanSource}`);
  if (!fs.existsSync(backupPath)) fs.copyFileSync(glbPath, backupPath);

  fs.copyFileSync(cleanSource, glbPath);
  const { json, bin } = readGlb(glbPath);
  json.bufferViews = json.bufferViews || [];
  json.accessors = json.accessors || [];
  const parts = [bin];

  const times = [0, 6, 8, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24];
  const timeAccessor = appendAccessor(json, parts, times, 'SCALAR', [0], [24]);

  const fingerNodes = [
    'rp_nathan_animated_003_walking_thumb_01_l',
    'rp_nathan_animated_003_walking_thumb_02_l',
    'rp_nathan_animated_003_walking_thumb_03_l',
    'rp_nathan_animated_003_walking_index_01_l',
    'rp_nathan_animated_003_walking_index_02_l',
    'rp_nathan_animated_003_walking_index_03_l',
    'rp_nathan_animated_003_walking_middle_01_l',
    'rp_nathan_animated_003_walking_middle_02_l',
    'rp_nathan_animated_003_walking_middle_03_l',
    'rp_nathan_animated_003_walking_ring_01_l',
    'rp_nathan_animated_003_walking_ring_02_l',
    'rp_nathan_animated_003_walking_ring_03_l',
    'rp_nathan_animated_003_walking_pinky_01_l',
    'rp_nathan_animated_003_walking_pinky_02_l',
    'rp_nathan_animated_003_walking_pinky_03_l',
  ];
  const armNodes = [
    'rp_nathan_animated_003_walking_shoulder_l',
    'rp_nathan_animated_003_walking_upperarm_l',
    'rp_nathan_animated_003_walking_lowerarm_l',
    'rp_nathan_animated_003_walking_hand_l',
    'rp_nathan_animated_003_walking_shoulder_r',
    'rp_nathan_animated_003_walking_upperarm_r',
    'rp_nathan_animated_003_walking_lowerarm_r',
    'rp_nathan_animated_003_walking_hand_r',
  ];
  const replacedNodes = new Set(['rp_nathan_animated_003_walking_root', ...fingerNodes, ...armNodes].map(name => nodeIndex(json, name)));
  const baseAnimation = json.animations && json.animations[0];
  const samplers = baseAnimation ? baseAnimation.samplers.slice() : [];
  const channels = baseAnimation
    ? baseAnimation.channels.filter(channel => !replacedNodes.has(channel.target.node))
    : [];

  function addRotation(node, keys) {
    const output = appendAccessor(json, parts, keys.flatMap(normalize), 'VEC4');
    samplers.push({ input: timeAccessor, interpolation: 'LINEAR', output });
    channels.push({ sampler: samplers.length - 1, target: { node: nodeIndex(json, node), path: 'rotation' } });
  }

  function addTranslation(node, keys) {
    const output = appendAccessor(json, parts, keys.flat(), 'VEC3');
    samplers.push({ input: timeAccessor, interpolation: 'LINEAR', output });
    channels.push({ sampler: samplers.length - 1, target: { node: nodeIndex(json, node), path: 'translation' } });
  }

  const open = qz(0);
  const thumbClosed = [
    qmul(qy(0.78), qz(0.58)),
    qmul(qy(0.48), qz(0.45)),
    qmul(qy(0.30), qz(0.28)),
  ];
  const closed = [qz(1.85), qz(2.15), qz(1.80)];

  // Finger closes one by one: thumb, index, middle, ring, pinky.
  // Then opens one by one in the same order.
  function fingerTimeline(fingerIndex, closedPose) {
    const closeAt = 3 + fingerIndex;
    const openAt = 8 + fingerIndex;
    return times.map((_, i) => (i >= closeAt && i < openAt ? closedPose : open));
  }

  addRotation('rp_nathan_animated_003_walking_thumb_01_l', fingerTimeline(0, thumbClosed[0]));
  addRotation('rp_nathan_animated_003_walking_thumb_02_l', fingerTimeline(0, thumbClosed[1]));
  addRotation('rp_nathan_animated_003_walking_thumb_03_l', fingerTimeline(0, thumbClosed[2]));

  const fingers = ['index', 'middle', 'ring', 'pinky'];
  fingers.forEach((finger, fingerOffset) => {
    const idx = fingerOffset + 1;
    addRotation(`rp_nathan_animated_003_walking_${finger}_01_l`, fingerTimeline(idx, closed[0]));
    addRotation(`rp_nathan_animated_003_walking_${finger}_02_l`, fingerTimeline(idx, closed[1]));
    addRotation(`rp_nathan_animated_003_walking_${finger}_03_l`, fingerTimeline(idx, closed[2]));
  });

  const leftRest = {
    shoulder: [-0.0413, 0.7971, -0.0034, 0.6025],
    upper: [-0.0166, 0.5708, -0.0492, 0.8195],
    lower: [0.2203, -0.0159, -0.1310, 0.9665],
    hand: [-0.7946, -0.0376, -0.0147, 0.6058],
  };
  const rightRest = {
    shoulder: [-0.7775, -0.0247, 0.6284, 0.0059],
    upper: [-0.0550, 0.5831, 0.0153, 0.8104],
    lower: [0.2685, -0.0433, -0.1517, 0.9503],
    hand: [-0.6951, 0.0099, 0.0360, 0.7180],
  };
  const leftForward = {
    shoulder: leftRest.shoulder,
    upper: [-0.0200, 0.1100, -0.7000, 0.7050],
    lower: [0.0600, -0.0100, -0.0200, 0.9980],
    hand: [-0.7200, -0.0100, -0.0200, 0.6940],
  };
  const rightForward = {
    shoulder: rightRest.shoulder,
    upper: [0.0200, 0.1100, 0.7000, 0.7050],
    lower: [0.0600, -0.0100, 0.0200, 0.9980],
    hand: [-0.7200, -0.0100, 0.0200, 0.6940],
  };
  function armTimeline(rest, forward) {
    return times.map((_, i) => (i < 2 ? rest : forward));
  }

  addRotation('rp_nathan_animated_003_walking_shoulder_l', armTimeline(leftRest.shoulder, leftForward.shoulder));
  addRotation('rp_nathan_animated_003_walking_upperarm_l', armTimeline(leftRest.upper, leftForward.upper));
  addRotation('rp_nathan_animated_003_walking_lowerarm_l', armTimeline(leftRest.lower, leftForward.lower));
  addRotation('rp_nathan_animated_003_walking_hand_l', armTimeline(leftRest.hand, leftForward.hand));
  addRotation('rp_nathan_animated_003_walking_shoulder_r', armTimeline(rightRest.shoulder, rightForward.shoulder));
  addRotation('rp_nathan_animated_003_walking_upperarm_r', armTimeline(rightRest.upper, rightForward.upper));
  addRotation('rp_nathan_animated_003_walking_lowerarm_r', armTimeline(rightRest.lower, rightForward.lower));
  addRotation('rp_nathan_animated_003_walking_hand_r', armTimeline(rightRest.hand, rightForward.hand));

  addTranslation('rp_nathan_animated_003_walking_root', [
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
    [0, 0, 0],
  ]);

  const finalBin = Buffer.concat(parts);
  json.buffers[0].byteLength = finalBin.length;
  json.animations = [{ name: 'Walk Stop Finger Close Open', samplers, channels }];
  writeGlb(glbPath, json, finalBin);
  console.log('Wrote walk-stop finger sequence');
  console.log('States: Walk in place 0-6s, Stop and both arms forward 6-10s, Close fingers 10-14s, Open fingers 16-20s');
  console.log(`Animation channels ${channels.length}`);
}

main();
