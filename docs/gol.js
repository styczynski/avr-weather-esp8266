window.installGol = (function(){

var CanvasHandler, CanvasHandlerPromise, GOLMap, aliveColour, c, cCnt, compileCodeToConditioner, condNormal, condSmile, cond, conds, deadColour, escape, g, golUtils, handleCanvas, makeSomeStuff, map, repaintCanvas, step, t, val, waitForFinalEvent;

aliveColour = [150, 150, 150, 1];

deadColour = [66, 66, 66, 0.1];

waitForFinalEvent = (function() {
  var timers;
  timers = {};
  return function(callback, ms, uniqueId) {
    if (!uniqueId) {
      uniqueId = "Don't call this twice without a uniqueId";
    }
    if (timers[uniqueId]) {
      clearTimeout(timers[uniqueId]);
    }
    return timers[uniqueId] = setTimeout(callback, ms);
  };
})();

Array.prototype.unique = function() {
  var a, i, j, m, n, ref, ref1, ref2;
  a = this.concat();
  for (i = m = 0, ref = a.length - 1; m <= ref; i = m += 1) {
    for (j = n = ref1 = i + 1, ref2 = a.length - 1; n <= ref2; j = n += 1) {
      if (a[i] === a[j]) {
        a.splice(j--, 1);
      }
    }
  }
  return a;
};

escape = function(s) {
  return s.replace(/[-\/\\^$*+?.()|[\]{}]/g, '\\$&');
};

CanvasHandlerPromise = (function() {
  CanvasHandlerPromise.prototype.bodyFn = null;

  CanvasHandlerPromise.prototype.next = null;

  CanvasHandlerPromise.prototype.root = null;

  function CanvasHandlerPromise(root, parent, bodyFn) {
    this.root = root;
    this.bodyFn = bodyFn;
  }

  CanvasHandlerPromise.prototype.flush = function() {
    return this.root.canvasHandler.flush();
  };

  CanvasHandlerPromise.prototype.call = function(label) {
    var tgt;
    tgt = this.root.promiseLabels[label];
    if (tgt != null) {
      this.next = tgt;
      return this.next;
    } else {
      return this;
    }
  };

  CanvasHandlerPromise.prototype.label = function(label) {
    return this.root.promiseLabels[label] = this;
  };

  CanvasHandlerPromise.prototype.workOn = function(label) {
    var tgt;
    tgt = this.root.promiseLabels[label];
    if (tgt != null) {
      return tgt;
    } else {
      return this;
    }
  };

  CanvasHandlerPromise.prototype.on = function(obj, event) {
    var capturedCanvas;
    capturedCanvas = null;
    this.next = new CanvasHandlerPromise(this.root, this, function(c) {
      return capturedCanvas = c;
    });
    ($(obj)).on(event, (function(_this) {
      return function() {
        return waitForFinalEvent(function() {
          if (capturedCanvas != null) {
            capturedCanvas.update();
            return _this.next.execute(capturedCanvas);
          }
        }, 500, "CHPromiseWindowResize");
      };
    })(this));
    return this.next;
  };

  CanvasHandlerPromise.prototype.paint = function(fn) {
    this.next = new CanvasHandlerPromise(this.root, this, fn);
    return this.next;
  };

  CanvasHandlerPromise.prototype.execute = function(c) {
    var ref, ref1, ref2;
    if (c == null) {
      c = (ref = this.root) != null ? (ref1 = ref.canvasHandler) != null ? ref1.canvas : void 0 : void 0;
    }
    if (this.bodyFn != null) {
      if (this.bodyFn.paint != null) {
        this.bodyFn.paint(c);
      } else {
        this.bodyFn(c);
      }
    }
    return (ref2 = this.next) != null ? ref2.execute(c) : void 0;
  };

  return CanvasHandlerPromise;

})();

CanvasHandler = (function() {
  CanvasHandler.prototype.canvas = null;

  CanvasHandler.prototype.paintQueue = null;

  CanvasHandler.prototype.props = null;

  CanvasHandler.prototype.flush = function() {
    this.canvas.update();
    return this.paintQueue.execute(this.canvas);
  };

  CanvasHandler.prototype.ready = function() {
    return this.paintQueue;
  };

  CanvasHandler.prototype.updateCanvas = function() {
    var c, ctx, props;
    c = $(this.props.id);
    ctx = c[0].getContext("2d");
    props = this.props;
    this.canvas = {
      x: ctx,
      dom: c,
      w: 0,
      h: 0,
      update: function() {
        this.w = props.w();
        this.h = props.h();
        this.x.canvas.width = this.w;
        return this.x.canvas.height = this.h;
      }
    };
    return this.canvas.update();
  };

  function CanvasHandler(props1) {
    this.props = props1;
    this.paintQueue = new CanvasHandlerPromise({
      canvasHandler: this,
      promiseLabels: []
    }, null, function() {});
    ($(document)).ready((function(_this) {
      return function() {
        _this.updateCanvas();
        return _this.flush();
      };
    })(this));
  }

  return CanvasHandler;

})();

handleCanvas = function(props) {
  var h;
  h = new CanvasHandler(props);
  window.canvasHandler = h;
  return h.ready();
};

repaintCanvas = function() {
  return window.canvasHandler.flush();
};

GOLMap = (function() {
  GOLMap.prototype.map = null;

  GOLMap.prototype.mapBuffer = null;

  GOLMap.prototype.alphaMask = null;

  GOLMap.prototype.w = 0;

  GOLMap.prototype.h = 0;

  GOLMap.prototype.conditioner = null;

  GOLMap.prototype.initMatrix = function() {
    var m, n, ref, ref1, v, x, y;
    v = [];
    for (x = m = 0, ref = this.w; m <= ref; x = m += 1) {
      v.push([]);
      for (y = n = 0, ref1 = this.h; n <= ref1; y = n += 1) {
        v[x].push(0);
      }
    }
    return v;
  };

  function GOLMap(w, h1, conditioner1) {
    var f_x, f_y, fi, fi_lower_l, fi_lower_rl, fi_upper_l, fi_upper_rl, m, map_c_x, map_c_y, map_maxd, n, ref, ref1, x, y;
    this.w = w;
    this.h = h1;
    this.conditioner = conditioner1;
    this.map = this.initMatrix();
    this.mapBuffer = this.initMatrix();
    this.alphaMask = this.initMatrix();
    map_c_x = this.w / 2.0;
    map_c_y = this.h / 2.0;
    f_x = 1.0;
    f_y = 3.0;
    fi_lower_l = 0.0;
    fi_upper_l = 1.0;
    fi_upper_rl = 0.9;
    fi_lower_rl = 0.0;
    map_maxd = Math.sqrt(f_x * map_c_x * map_c_x + f_y * map_c_y * map_c_y + 0.1) * 0.7;
    for (x = m = 0, ref = this.w; m <= ref; x = m += 1) {
      for (y = n = 0, ref1 = this.h; n <= ref1; y = n += 1) {
        fi = 1 - (Math.sqrt(f_x * (x - map_c_x) * (x - map_c_x) + f_y * (y - map_c_y) * (y - map_c_y)) / map_maxd);
        if (fi > fi_upper_rl) {
          fi = fi_upper_l;
        }
        if (fi < fi_lower_rl) {
          fi = fi_lower_l;
        }
        fi = Math.max(fi_lower_l, fi);
        fi = Math.min(fi_upper_l, fi);
        this.alphaMask[x][y] = fi;
      }
    }
  }

  GOLMap.prototype.set = function(x, y, name) {
    if (this.map[x] != null) {
      if (this.map[x][y] != null) {
        return this.map[x][y] = this.conditioner.translateStateName(name);
      }
    }
  };

  GOLMap.prototype.drawStructure = function(x, y, obj) {
    var k, results, v;
    results = [];
    for (k in obj) {
      v = obj[k];
      k = k.split(",");
      results.push(this.set((parseInt(k[0])) + x, (parseInt(k[1])) + y, v));
    }
    return results;
  };

  GOLMap.prototype.step = function(movx, movy) {
    var m, n, o, ref, ref1, ref2, ref3, results, x, y;
    if (movx == null) {
      movx = 0;
    }
    if (movy == null) {
      movy = 0;
    }
    for (x = m = 0, ref = this.w; m <= ref; x = m += 1) {
      for (y = n = 0, ref1 = this.h; n <= ref1; y = n += 1) {
        this.mapBuffer[x][y] = this.conditioner.step(x, y, this.map);
      }
    }
    results = [];
    for (x = o = ref2 = movx, ref3 = this.w - movx; o <= ref3; x = o += 1) {
      results.push((function() {
        var q, ref4, ref5, results1;
        results1 = [];
        for (y = q = ref4 = movy, ref5 = this.h - movy; q <= ref5; y = q += 1) {
          results1.push(this.map[x][y] = this.mapBuffer[x + movx][y + movy]);
        }
        return results1;
      }).call(this));
    }
    return results;
  };

  GOLMap.prototype.paint = function(c) {
    var field_h, field_margin_left, field_margin_top, field_rh, field_rw, field_w, m, ref, results, rgba, shift_x, shift_y, toth, totw, x, y, zoom;
    zoom = 1.5;
    field_margin_left = 2;
    field_margin_top = 2;
    field_w = (c.w * zoom) / this.w;
    field_h = (c.h * zoom) / this.h;
    field_w = Math.min(field_w, field_h);
    field_h = field_w;
    totw = field_w * this.w;
    toth = field_h * this.h;
    shift_x = (c.w - totw) / 2.0;
    shift_y = (c.h - toth) / 2.0;
    field_rw = field_w - field_margin_left;
    field_rh = field_h - field_margin_top;
    results = [];
    for (x = m = 0, ref = this.w; m <= ref; x = m += 1) {
      results.push((function() {
        var n, ref1, results1;
        results1 = [];
        for (y = n = 0, ref1 = this.h; n <= ref1; y = n += 1) {
          c.x.save();
          c.x.translate(x * field_w + shift_x, y * field_h + shift_y);
          rgba = this.conditioner.colour(this.map[x][y]);
          rgba[3] *= this.alphaMask[x][y];
          c.x.fillStyle = "rgba(" + (rgba.join(',')) + ")";
          c.x.fillRect(-field_rw, -field_rh, field_rw, field_rh);
          results1.push(c.x.restore());
        }
        return results1;
      }).call(this));
    }
    return results;
  };

  return GOLMap;

})();

golUtils = {
  count: function(l) {
    return l.length;
  },
  rangeCircle: function(map, x0, y0, r) {
    var m, n, ref, ref1, ref2, ref3, ret, x, y;
    ret = [];
    for (x = m = ref = x0 - r, ref1 = x0 + r; m <= ref1; x = m += 1) {
      for (y = n = ref2 = y0 - r, ref3 = y0 + r; n <= ref3; y = n += 1) {
        if ((x - x0) * (x - x0) + (y - y0) * (y - y0) <= r * r) {
          if (map[x] != null) {
            if (map[x][y] != null) {
              ret.push(map[x][y]);
            }
          }
        }
      }
    }
    return ret;
  },
  rangeDistStraight: function(map, x0, y0, r) {
    var m, n, ref, ref1, ref2, ref3, ret, x, y;
    ret = [];
    for (x = m = ref = x0 - r, ref1 = x0 + r; m <= ref1; x = m += 1) {
      for (y = n = ref2 = y0 - r, ref3 = y0 + r; n <= ref3; y = n += 1) {
        if (((x - x0) * (x - x0) + (y - y0) * (y - y0)) === (r * r)) {
          if (map[x] != null) {
            if (map[x][y] != null) {
              ret.push(map[x][y]);
            }
          }
        }
      }
    }
    return ret;
  },
  rangeDist: function(map, x0, y0, r) {
    var d, m, n, ref, ref1, ref2, ref3, ret, x, y;
    ret = [];
    for (x = m = ref = x0 - r, ref1 = x0 + r; m <= ref1; x = m += 1) {
      for (y = n = ref2 = y0 - r, ref3 = y0 + r; n <= ref3; y = n += 1) {
        d = (x - x0) * (x - x0) + (y - y0) * (y - y0);
        if (d >= (r * r) && d < ((r + 1) * (r + 1))) {
          if (map[x] != null) {
            if (map[x][y] != null) {
              ret.push(map[x][y]);
            }
          }
        }
      }
    }
    return ret;
  },
  filter: function(l, p) {
    return l.filter(p);
  }
};

compileCodeToConditioner = function(code) {
  var A, B, C, alias, conditioner, defaultDecl, defaultStateDecl, dir, dir1, dir2, fn, i, len, len1, len2, len3, len4, len5, m, match, matchDecl, matchStatesDecl, n, ncode, o, prfdecl, q, spl, statesNames, statesNamesAliases, u, varDecls, w;
  ncode = "";
  defaultDecl = null;
  defaultStateDecl = /^([ a-zA-Z0-9->,]+?)$/gm;
  match = defaultStateDecl.exec(code);
  if (match !== null) {
    defaultDecl = match[0];
    code = code + ("\n" + defaultDecl + ": true\n");
  }
  matchDecl = /([ a-zA-Z0-9->,]+?):(.*)/gm;
  match = matchDecl.exec(code);
  varDecls = "";
  while (match !== null) {
    if (match[1].indexOf("set ") !== -1) {
      spl = (match[1].split(" "))[1];
      varDecls += "var " + spl + " = " + match[2] + ";\n";
    } else {
      ncode += match[1] + ": " + match[2] + "\n";
    }
    match = matchDecl.exec(code);
  }
  matchStatesDecl = /([a-zA-Z0-9->,]+?):(.*)/gm;
  code = ncode;
  statesNames = [];
  statesNamesAliases = {};
  match = matchStatesDecl.exec(code);
  while (match !== null) {
    statesNames = statesNames.concat(match[1].split(/->|,/g)).unique();
    match = matchStatesDecl.exec(code);
  }
  statesNames = statesNames.sort(function(a, b) {
    if (a.length === b.length) {
      return b - a;
    } else {
      return b.length - a.length;
    }
  });
  if (defaultDecl !== null) {
    statesNamesAliases[defaultDecl] = 0;
    i = 1;
    for (m = 0, len = statesNames.length; m < len; m++) {
      alias = statesNames[m];
      if (alias !== defaultDecl) {
        statesNamesAliases[alias] = i;
        ++i;
      }
    }
  } else {
    i = 0;
    for (n = 0, len1 = statesNames.length; n < len1; n++) {
      alias = statesNames[n];
      statesNamesAliases[alias] = i;
      ++i;
    }
  }
  match = matchStatesDecl.exec(code);
  ncode = "";
  while (match !== null) {
    dir = match[1].split("->");
    C = match[2].trim();
    if (dir.length === 2) {
      dir1 = dir[0].split(",");
      dir2 = dir[1].split(",");
      for (o = 0, len2 = dir1.length; o < len2; o++) {
        A = dir1[o];
        for (q = 0, len3 = dir2.length; q < len3; q++) {
          B = dir2[q];
          ncode += B + ": is(" + A + ") && (" + C + ")\n";
        }
      }
    } else {
      dir1 = dir[0].split(",");
      for (u = 0, len4 = dir1.length; u < len4; u++) {
        A = dir1[u];
        ncode += A + ": " + C + "\n";
      }
    }
    match = matchStatesDecl.exec(code);
  }
  code = ncode;
  match = matchStatesDecl.exec(code);
  ncode = "";
  while (match !== null) {
    B = match[2].trim();
    A = match[1].trim();
    ncode += "if(" + B + ") {return (" + A + ");}\n";
    match = matchStatesDecl.exec(code);
  }
  code = ncode;
  prfdecl = "";
  for (w = 0, len5 = statesNames.length; w < len5; w++) {
    alias = statesNames[w];
    prfdecl += "var " + alias + " = " + statesNamesAliases[alias] + ";\n";
  }
  prfdecl += "var self = map[x][y];\nvar get = function(x0, y0) {\n  if(map[x0] === undefined) {\n    return null;\n  }\n  if(map[x0][y0] === undefined) {\n    return null;\n  }\n  return map[x0][y0];\n};\nvar filterValue = function(t, v) { return t.filter(function(e){return e === v;}); };\nvar circle = function(r) { return golUtils.rangeCircle(map,x,y,r); };\nvar countCircleValues = function(r, v) { return (filterValue(circle(r),v)).length; };\nvar range = function(r) { return golUtils.rangeDist(map,x,y,r); };\nvar countRangeValues = function(r, v) { return (filterValue(range(r),v)).length; };\nvar rangeStraight = function(r) { return golUtils.rangeDistStraight(map,x,y,r); };\nvar countRangeStraightValues = function(r, v) { return (filterValue(rangeStraight(r),v)).length; };\nvar is = function(o, value) {\n  if(value === undefined) {\n    return map[x][y] === o;\n  }\n  return o === value;\n};\nvar $ = function(xr, yr) { return map[xr+x][yr+y]; };";
  code = prfdecl + "\n" + varDecls + code;
  conditioner = {
    colour: function(v) {
      v = v % this.states;
      if(v==0) {
        return [255.0, 255.0, 255.0, 0.0];
      } else {
        return [255.0, 255.0, 255.0, 1.0];
      }
      //return [Math.min(aliveColour[0], deadColour[0]) + (v / (this.states - 1)) * Math.abs(aliveColour[0] - deadColour[0]), Math.min(aliveColour[1], deadColour[1]) + (v / (this.states - 1)) * Math.abs(aliveColour[1] - deadColour[1]), Math.min(aliveColour[2], deadColour[2]) + (v / (this.states - 1)) * Math.abs(aliveColour[2] - deadColour[2]), 1.0];
    }
  };
  conditioner.translateStateName = function(name) {
    if (statesNamesAliases[name] == null) {
      if (defaultDecl !== null) {
        return 0;
      } else {
        return 0;
      }
    }
    return statesNamesAliases[name];
  };
  conditioner.states = statesNames.length;
  code = "(function(x, y, map){\n" + code + "})";
  fn = eval(code);
  conditioner.step = fn;
  return conditioner;
};

cond = compileCodeToConditioner("set k: countRangeValues(1, Live)\nLive->Dead: k<2\nLive->Dead: k>3\nLive->Live: k == 2 || k == 3\nDead->Live: k == 3\nDead");
val = "Live";

map = new GOLMap(80, 5, cond);

conds = [
  {
    c: cond,
    needBump: true
  }
];

c = handleCanvas({
  id: '#golcanvas',
  w: function() {
    return 1200;
  },
  h: function() {
    return 40;
  }
});

makeSomeStuff = function(strength) {
  var box_x_rng, box_y_rng, i, j, m, num, prb, ref, results, x0, y0;
  box_x_rng = [3, 75];
  box_y_rng = [1, 4];
  prb = Math.random() * 10;
  results = [];
  for (j = m = 0, ref = prb; m <= ref; j = m += 1) {
    num = Math.random() * strength + 10;
    results.push((function() {
      var n, ref1, results1;
      results1 = [];
      for (i = n = 0, ref1 = num; n <= ref1; i = n += 1) {
        if (Math.random() > 0.5) {
          x0 = Math.random() * (box_x_rng[1] - box_x_rng[0]) + box_x_rng[0];
          y0 = Math.random() * (box_y_rng[1] - box_y_rng[0]) + box_y_rng[0];
          x0 = parseInt(x0);
          y0 = parseInt(y0);
          results1.push(map.set(x0, y0, val));
        } else {
          results1.push(void 0);
        }
      }
      return results1;
    })());
  }
  return results;
};

t = 101;

g = 101;

cCnt = parseInt(Math.random() * conds.length * 2) % conds.length;

step = function() {
  var s;
  if (g > 1) {
    cCnt++;
    cCnt = cCnt % conds.length;
    map.conditioner = conds[cCnt].c;
    g = 0;
    t = 0;
    if (conds[cCnt].needBump) {
      makeSomeStuff(100);
    }
  }
  if (t > 10) {
    if (conds[cCnt].needBump) {
      makeSomeStuff(10);
    }
    ++g;
    t = 0;
  }
  s = 0;
  map.step(s, s);
  c.flush();
  return ++t;
};

window.step = function() {
  return step();
};

setInterval(function() {
  return step();
}, 500);

c.on(window, 'resize').paint(function(c) {
  c.x.fillStyle = 'rgba(0, 45, 66, 0.1)';
  return c.x.fillRect(0, 0, c.w, c.height);
}).paint(map);

});
