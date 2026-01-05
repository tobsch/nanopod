# NanoPod Oak Stand - Design Documentation

A wooden stand for the CrowPanel 1.28" ESP32 display with angled viewing surface.

## Specifications

| Parameter | Value |
|-----------|-------|
| **Base dimensions** | 80 x 80 mm |
| **Front height** | 15 mm |
| **Back height** | 36.4 mm |
| **Viewing angle** | 15° |
| **Pocket diameter** | 55 mm |
| **Pocket depth** | 18 mm (perpendicular to surface) |
| **Edge chamfers** | 1 mm (45° cut) |
| **Material** | Oak wood (recommended) |

## Views

### Side View (Right Side)
```
                        ┌─┐
                       ╱  │
                      ╱   │
                     ╱    │
                    ╱     │
                   ╱  ○   │ 36.4mm
                  ╱       │ (back)
                 ╱        │
                ╱ 15°     │
               ╱__________│
              │           │ 15mm
              └───────────┘ (front)
              ←── 80mm ──→
```

### Front View
```
              ┌───────────────────┐
             ╱                     ╲  ← 1mm chamfer
            │                       │
            │     ╭───────────╮     │
            │    ╱             ╲    │  ← 1mm pocket bevel
            │   │               │   │
            │   │    55mm dia   │   │
            │   │               │   │
            │    ╲             ╱    │
            │     ╰───────────╯     │
            │                       │
             ╲                     ╱  ← 1mm chamfer
              └───────────────────┘
              ←────── 80mm ──────→
```

### Top View
```
              ┌───────────────────┐
              │                   │
              │                   │
              │     ┌───────┐     │
              │     │       │     │
              │     │   ○   │     │  ← 55mm pocket, centered
              │     │       │     │
              │     └───────┘     │
              │                   │
              │                   │
              └───────────────────┘
              ←────── 80mm ──────→

                    ↑
                   80mm
                    ↓
```

### Cross-Section (Side Cut Through Center)
```
                     ╱│
                   ╱  │
                 ╱    │
               ╱══════│ ← 55mm pocket opening (with 1mm bevel)
             ╱   18mm │   pocket depth perpendicular to surface
           ╱══════════│
         ╱            │
       ╱   15°        │ 36.4mm
     ╱________________│
    │                 │ 15mm
    └─────────────────┘
         80mm
```

### Pocket Detail (Cross-Section)
```
    Surface (15° angle)
         ╲
          ╲  1mm chamfer
           ╲_
            │ │
            │ │
            │ │  18mm deep
            │ │  55mm diameter
            │ │
            └─┘  (flat bottom)

    Pocket axis is perpendicular
    to the angled surface (15° from vertical)
```

## Key Measurements for Manual Construction

### Starting Block
- Minimum size: **80 x 80 x 40 mm** oak block

### Step 1: Cut the Wedge Profile
Cut the angled top surface:
- Front edge height: **15 mm**
- Back edge height: **36.4 mm** (or 36.5 mm for tolerance)
- Angle: **15°** from horizontal

### Step 2: Drill the Pocket
Position the pocket at the **center** of the angled face:
- Center point: **40 mm** from front, **40 mm** from sides
- Surface height at center: **~25.7 mm** from bottom
- Drill **perpendicular to the angled surface** (15° from vertical, tilted toward front)
- Diameter: **55 mm** (use Forstner bit)
- Depth: **18 mm**

### Step 3: Chamfer the Pocket Edge
- Use a **1 mm 45° chamfer bit** or hand tool
- Chamfer around the top edge of the pocket opening

### Step 4: Chamfer All Long Edges
Apply **1 mm 45° chamfer** to:
- 4 bottom edges
- 4 vertical corner edges
- 2 top edges along the angled surface (left and right sides)

### Step 5: Drill Cable Hole (Optional)
If needed for cable routing:
- **6 mm** hole from center of pocket straight to back
- Drill horizontally (parallel to bottom)

## Angle Reference

To set up a 15° angle:
- Rise over run: **26.8 mm rise per 100 mm run**
- Or: **21.4 mm rise over 80 mm depth**
- Tangent: **tan(15°) = 0.2679**

## Tool Suggestions

| Operation | Tool |
|-----------|------|
| Wedge cut | Table saw with blade tilted 15°, or bandsaw + hand plane |
| Pocket | 55mm Forstner bit in drill press (tilted 15°) |
| Pocket chamfer | Chamfer bit in router or hand chamfer tool |
| Edge chamfers | Router with 45° chamfer bit, or hand plane/file |
| Cable hole | 6mm drill bit |

## Files

- `stand.scad` - OpenSCAD parametric 3D model
- `stand.stl` - Export from OpenSCAD (F6 then File > Export > STL)

## OpenSCAD Export

```bash
# Open in OpenSCAD
open stand.scad

# In OpenSCAD:
# 1. Press F6 (full render)
# 2. File > Export > Export as STL
```
