
/*
1. Configure the video card for the desired graphics mode
▶ Minix 3 boots in text mode, not in graphics mode
2. Write to VRAM to display on the screen what is requested
▶ Map VRAM to the process’ address space
3. Reset the video card to the text mode used by Minix
▶ You need only to call a function that we provide you
*/


// Map the physical memory of the frame buffer
/*
int r;
struct minix_mem_range mr; // physical memory range 
unsigned int vram_base; // VRAM’s physical addresss 
unsigned int vram_size; // VRAM’s size, but you can use the frame-buffer size, instead
void *video_mem; // frame-buffer VM address 
// Allow memory mapping
mr.mr_base = (phys_bytes) vram_base;
mr.mr_limit = mr.mr_base + vram_size;
if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
panic("sys_privctl (ADD_MEM) failed: %d\n", r);
// Map memory
video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
if(video_mem == MAP_FAILED)
panic("couldn’t map video memory");

*/


// Change the frame buffer to what is wanted
/*
static void *video_mem; // Address to which VRAM is mapped
static unsigned hres; // Frame horizontal resolution
static unsigned vres; // Frame vertical resolution
void vg_fill(uint32_t color) {
int i;
uint32_t *ptr; // Assuming 4 byte per pixel
ptr = video_mem;
for(i = 0; i< hres*vres; i++, ptr++) {
 // Handle a pixel at a time

*/