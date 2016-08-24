#include "gimp-2.0/libgimp/gimp.h"
#include "CL/cl.h"
#include "stdio.h"
#include "string.h"
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#define MAX_SOURCE_SIZE (0x100000)

#define LOCAL_SIZE 8


typedef struct
{
  gint radius;
} MyBlurVals;

static void query (void);
static void run   (const gchar      *name,
                   gint              nparams,
                   const GimpParam  *param,
                   gint             *nreturn_vals,
                   GimpParam       **return_vals);
static void blur(GimpDrawable *drawable);
static gboolean blur_dialog (GimpDrawable *drawable);
/* Set up default values for options */
static MyBlurVals bvals =
{
  5  /* radius */
};

GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,
  NULL,
  query,
  run
};

MAIN()

static void
query (void)
{
  static GimpParamDef args[] =
  {
    {
      GIMP_PDB_INT32,
      "run-mode",
      "Run mode"
    },
    {
      GIMP_PDB_IMAGE,
      "image",
      "Input image"
    },
    {
      GIMP_PDB_DRAWABLE,
      "drawable",
      "Input drawable"
    }
  };

    gimp_install_procedure (
    "plug-in-myblur4",
    "My blur 4 (UI)",
    "Blurs the image",
    "David Neary",
    "Copyright David Neary",
    "2004",
    "_My blur 4 (UI)...",
    "RGB*,GRAY*",
    GIMP_PLUGIN,
    G_N_ELEMENTS (args), 0,
    args, NULL);

  gimp_plugin_menu_register ("plug-in-myblur4",
                             "<Image>/Filters/Blur");
}

static void
run (const gchar      *name,
     gint              nparams,
     const GimpParam  *param,
     gint             *nreturn_vals,
     GimpParam       **return_vals){

  static GimpParam  values[1];
        GimpPDBStatusType status = GIMP_PDB_SUCCESS;
        GimpRunMode       run_mode;
        GimpDrawable     *drawable;

	fprintf(stderr, "Start.\n");
        /* Setting mandatory output values */
        *nreturn_vals = 1;
        *return_vals  = values;

        values[0].type = GIMP_PDB_STATUS;
        values[0].data.d_status = status;

        /* Getting run_mode - we won't display a dialog if 
         * we are in NONINTERACTIVE mode */
        run_mode = param[0].data.d_int32;

        /*  Get the specified drawable  */
        drawable = gimp_drawable_get (param[2].data.d_drawable);

        gimp_progress_init ("My Blur...");


        GTimer* timer;
	timer= g_timer_new();
         
  	switch (run_mode){
    	    case GIMP_RUN_INTERACTIVE:
	      /* Get options last values if needed */
	      gimp_get_data ("plug-in-myblur", &bvals);

	      /* Display the dialog */
	      if (! blur_dialog (drawable))
		return;
	      break;

	    case GIMP_RUN_NONINTERACTIVE:
	      if (nparams != 4)
		status = GIMP_PDB_CALLING_ERROR;
	      if (status == GIMP_PDB_SUCCESS)
		bvals.radius = param[3].data.d_int32;
	      break;

	    case GIMP_RUN_WITH_LAST_VALS:
	      /*  Get options last values if needed  */
	      gimp_get_data ("plug-in-myblur", &bvals);
	      break;

	    default:
	      break;
	 }
        blur (drawable);

        g_print ("blur() took %g seconds.\n", g_timer_elapsed (timer,NULL));
        g_timer_destroy (timer);
         

        gimp_displays_flush ();
        gimp_drawable_detach (drawable);
	/*  Finally, set options in the core  */
	  if (run_mode == GIMP_RUN_INTERACTIVE)
	    gimp_set_data ("plug-in-myblur", &bvals, sizeof (MyBlurVals));

}
static void blur(GimpDrawable *drawable){

	//GIMP
	gint         i, j, k, channels;
        gint         x1, y1, x2, y2;
        GimpPixelRgn rgn_in, rgn_out;
        guchar      *rows;
	g_print ("init\n");
     	
	fprintf(stderr, "Start.\n");

	//OPENCL
	//kernel src
	FILE* fp;
	char *source_str;
    	size_t source_size;
	
	fp = fopen("/home/marek/.gimp-2.8/plug-ins/kernel.cl", "r");
    	if (!fp) {
       		fprintf(stderr, "Nie wczytano kernela.\n");
        	exit(1);
    	}
    	source_str = (char*)malloc(MAX_SOURCE_SIZE);
    	source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    	fclose( fp );
	

	if (gimp_drawable_has_alpha(drawable->gimp_drawable_get_image() ) == FALSE){
		fprintf(stderr, "dupa \n");
	}	
	
	cl_platform_id platform_id = NULL;
    	cl_device_id device_id = NULL;
    	cl_uint ret_num_devices;
    	cl_uint ret_num_platforms;

    	cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

    	ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 1,
                          &device_id, &ret_num_devices);

	g_print ("contex create\n");
    	cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
		
	g_print ("mask_bounds\n");
	gimp_drawable_mask_bounds (drawable->drawable_id,
                                   &x1, &y1,
                                   &x2, &y2);
        channels = gimp_drawable_bpp (drawable->drawable_id);

	gimp_pixel_rgn_init (&rgn_in,
                             drawable,
                             x1, y1,
                             x2 - x1, y2 - y1, 
                             FALSE, FALSE);

        gimp_pixel_rgn_init (&rgn_out,
                             drawable,
                             x1, y1,
                             x2 - x1, y2 - y1,
                             TRUE, TRUE);
	
	int width= x2-x1;
	int height = y2-y1;
	

	g_print ("cl_img format\n");
    	cl_image_format format;
    	format.image_channel_order = CL_RGBA;
    	format.image_channel_data_type = CL_UNSIGNED_INT8;
	
	cl_mem clImage1 = clCreateImage2D(context, CL_MEM_READ_ONLY  ,&format ,width, height,  0, rows, &ret);
    	cl_mem clImage2 = clCreateImage2D(context, CL_MEM_WRITE_ONLY ,&format,width, height, 0, 0, &ret);
	
	cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
	





	g_print ("origin\n");
	size_t origin[] = {0,0,0};
    	size_t region[] = {width, height,1};

	g_print ("rows allocate\n");
        rows = g_new (guchar,(y2-y1)*channels * (x2 - x1));

	g_print ("get rect\n");
	gimp_pixel_rgn_get_rect(&rgn_in,rows,x1,y1,x2,y2);
	
	//WYSŁANIE
	g_print ("write image %d %d\n",width,height);
	rows[2]=0xFF;
	g_print ("write image %d\n",ret);
		
	ret = clEnqueueWriteImage(command_queue, clImage1, CL_TRUE, origin, region,0,0, rows, 0, NULL,NULL );
	g_print ("create program\n");
	cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
	
	g_print ("build\n");
    	ret = clBuildProgram(program, 1, &device_id,NULL,NULL, NULL);

    	cl_kernel kernel = clCreateKernel(program, "mediana", &ret);
    	ret=clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&clImage1);
	ret=clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&clImage2);
	int step = bvals.radius;
	ret=clSetKernelArg(kernel, 2,sizeof(int),(void *)&step);
	ret=clSetKernelArg(kernel, 3,sizeof(int),(void *)&width);
	ret=clSetKernelArg(kernel, 4,sizeof(int),(void *)&height);
	int dod = ((step+1)*(step+1))/2;
	ret=clSetKernelArg(kernel, 5,sizeof(int),(void *)&dod);
	g_print ("gwsize\n");
	size_t GWSize[2]= {width+(LOCAL_SIZE-width%LOCAL_SIZE),height+(LOCAL_SIZE-height%LOCAL_SIZE)};
    	size_t local[2]= {LOCAL_SIZE,LOCAL_SIZE};
    	int dim=2;
        cl_event event;
	g_print ("enque kernel\n");
//WYWOŁANIE KERNELA!
	ret = clEnqueueNDRangeKernel(command_queue, kernel, dim, NULL, (const size_t *)GWSize, (const size_t *)local, 0, NULL, &event);	

	clWaitForEvents(1,&event);
    	ret = clEnqueueReadImage(command_queue, clImage2, CL_TRUE, origin, region, 0, 0, rows,0, NULL, NULL);
	
	//zapis
	gimp_pixel_rgn_set_rect(&rgn_out,rows,x1,y1,x2,y2);


        g_free (rows);


        gimp_drawable_flush (drawable);
        gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
        gimp_drawable_update (drawable->drawable_id,
                              x1, y1,
                              x2 - x1, y2 - y1);
	g_print ("end\n");
}
static gboolean blur_dialog (GimpDrawable *drawable)
{
  GtkWidget *dialog;
  GtkWidget *main_vbox;
  GtkWidget *main_hbox;
  GtkWidget *frame;
  GtkWidget *radius_label;
  GtkWidget *alignment;
  GtkWidget *spinbutton;
  GtkObject *spinbutton_adj;
  GtkWidget *frame_label;
  gboolean   run;

  gimp_ui_init ("myblur", FALSE);

  dialog = gimp_dialog_new ("My blur", "myblur",
                            NULL, 0,
                            gimp_standard_help_func, "plug-in-myblur",

                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                            GTK_STOCK_OK,     GTK_RESPONSE_OK,

                            NULL);

  main_vbox = gtk_vbox_new (FALSE, 6);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), main_vbox);
  gtk_widget_show (main_vbox);

  frame = gtk_frame_new (NULL);
  gtk_widget_show (frame);
  gtk_box_pack_start (GTK_BOX (main_vbox), frame, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 6);

  alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment);
  gtk_container_add (GTK_CONTAINER (frame), alignment);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment), 6, 6, 6, 6);

  main_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (main_hbox);
  gtk_container_add (GTK_CONTAINER (alignment), main_hbox);

  radius_label = gtk_label_new_with_mnemonic ("_Radius:");
  gtk_widget_show (radius_label);
  gtk_box_pack_start (GTK_BOX (main_hbox), radius_label, FALSE, FALSE, 6);
  gtk_label_set_justify (GTK_LABEL (radius_label), GTK_JUSTIFY_RIGHT);

  spinbutton_adj = gtk_adjustment_new (3, 1, 16, 1, 5, 5);
  spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_adj), 1, 0);
  gtk_widget_show (spinbutton);
  gtk_box_pack_start (GTK_BOX (main_hbox), spinbutton, FALSE, FALSE, 6);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton), TRUE);

  frame_label = gtk_label_new ("<b>Modify radius</b>");
  gtk_widget_show (frame_label);
  gtk_frame_set_label_widget (GTK_FRAME (frame), frame_label);
  gtk_label_set_use_markup (GTK_LABEL (frame_label), TRUE);

  g_signal_connect (spinbutton_adj, "value_changed",
                    G_CALLBACK (gimp_int_adjustment_update),
                    &bvals.radius);
  gtk_widget_show (dialog);

  run = (gimp_dialog_run (GIMP_DIALOG (dialog)) == GTK_RESPONSE_OK);

  gtk_widget_destroy (dialog);

  return run;
}


