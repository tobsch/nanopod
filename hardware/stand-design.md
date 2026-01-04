# NanoPod Oak Stand Design

## Specifications

| Feature | Value |
|---------|-------|
| Width | 120mm |
| Depth | 80mm |
| Front height | 10mm |
| Back height | 47mm |
| Angle | 25° |
| Pocket diameter | 55mm |
| Pocket depth | 17mm |
| Cable hole | 6mm (through) |
| Edge bevel | 2.5mm chamfer |
| Material | Oak |

## Views

### Side View
```
                    ╲
                   ╱ ╲
                 ╱════│  55mm pocket
               ╱  ○   │  17mm deep        47mm
             ╱════════│  6mm through-hole  back
           ╱          │
         ╱           ╱   25°
       ╱╲─────────╱╱
      │▓▓▓▓▓▓▓▓▓▓▓│ 10mm front
      ╲───────────╱
          80mm
```

### Front View
```
       ╲____________________╱
      ╱                      ╲
     │       ╭────────╮       │
     │      ╱          ╲      │
     │     │     ○      │     │  55mm pocket
     │      ╲   6mm    ╱      │
     │       ╰────────╯       │
     │                        │
     ╲▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓╱
       ╲____________________╱
              120mm
```

### Cross-Section (Pocket Detail)
```
      ╱                    ╱
    ╱  ┌────────────────┐╱
  ╱    │   55mm pocket  │     17mm deep
╱      │                │
       └──────┬──┬──────┘
              │  │            6mm cable hole
              │  │            (through)
──────────────┴──┴────────
```

## Files

- `stand.scad` - OpenSCAD parametric 3D model
- `stand.stl` - Exported mesh (generate with OpenSCAD)

## Rendering Instructions

### Install OpenSCAD
```bash
brew install openscad
```

### Generate STL
```bash
openscad -o stand.stl stand.scad
```

### Or open in GUI
```bash
open stand.scad
```
Then press F6 to render and File > Export > STL

## Manufacturing Notes

### CNC Routing
1. Start with 100mm thick oak block (or glue-up)
2. Mill outer wedge profile
3. Mill 55mm pocket with flat-bottom bit
4. Drill 6mm through-hole
5. Route 2.5mm chamfer on all edges
6. Sand to 220 grit, apply finish

### Hand Tools
1. Cut wedge blank on bandsaw
2. Plane angled face to 25°
3. Use Forstner bit (55mm) for pocket
4. Drill 6mm cable hole
5. Chamfer edges with router or hand plane
6. Sand and finish
