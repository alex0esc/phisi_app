
#define INTERPOLATE(cell_loc, indices, weights, multiplayer) \
    {\
    vec2 cell_loc_cpy = cell_loc; \
    uint x_traced = uint(cell_loc_cpy.x); \
    uint y_traced = uint(cell_loc_cpy.y); \
    float x_inner = cell_loc_cpy.x - float(x_traced); \
    float y_inner = cell_loc_cpy.y - float(y_traced); \
    indices[0] = (y_traced * width + x_traced); \
    indices[1] = (y_traced * width + (x_traced + 1)); \
    indices[2] = ((y_traced + 1) * width + (x_traced + 1)); \
    indices[3] = ((y_traced + 1) * width + x_traced); \
    weights[0] = (1.0 - x_inner) * (1.0 - y_inner) * WALL(indices[0]); \
    weights[1] = x_inner * (1.0 - y_inner) * WALL(indices[1]); \
    weights[2] = x_inner * y_inner * WALL(indices[2]); \
    weights[3] = (1.0 - x_inner) * y_inner * WALL(indices[3]); \
    multiplayer = 1.0 / (weights[0] + weights[1] + weights[2] + weights[3]); \
    }


float intersection(vec2 a, vec2 b, vec2 c, vec2 d) {
    float div = d.x * b.y - d.y * b.x; 
    if (div == 0.0)
      return 0.0;
    return ((c.y - a.y) * d.x - (c.x - a.x) * d.y) / div;
}
    
vec2 reflect_custom(vec2 a, vec2 b) {
    return 2 * (dot(a, b) / dot(b, b)) * b - a;  
}

void reflect_in_box(uint width, uint height, vec2 cell_vec, inout vec2 cell_loc) {
    while(true) {
       float factors[4];
       factors[0] = intersection(cell_loc, cell_vec, vec2(0.0, 1.0), vec2(1.0, 0.0)); 
       factors[1] = intersection(cell_loc, cell_vec, vec2(1.0, 0.0), vec2(0.0, 1.0));
       factors[2] = intersection(cell_loc, cell_vec, vec2(0.0, height - 1.0f), vec2(1.0, 0.0));     
       factors[3] = intersection(cell_loc, cell_vec, vec2(width - 1.0f, 0.0), vec2(0.0, 1.0));
       float smallest_value = 3.4028235e38;
       int smallest_index = -1;
       for (uint i = 0; i < 4; i++) {
         if (factors[i] > 1.0 || factors[i] <= 0.0 || factors[i] > smallest_value)
           continue;
         smallest_value = factors[i];
         smallest_index = int(i);
       }
       if(smallest_index == -1) {   
         cell_loc += cell_vec;
         break;
       }
       cell_loc += cell_vec * smallest_value * 0.9999;        
       if (smallest_index == 0 || smallest_index == 2) {  
         cell_vec = reflect_custom(cell_vec, vec2(1.0, 0.0)) * (1.0 - smallest_value);
       } else if (smallest_index == 1 || smallest_index == 3) {
         cell_vec = reflect_custom(cell_vec, vec2(0.0, 1.0)) * (1.0 - smallest_value);
       }
    }   
}

