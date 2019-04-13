In the first commit, the USB was only able to connect to the OS with its register and vendor id's.
Now I will try to add some I/O capabilities using USB Endpoints.

Some USB Endpoints and their types that might be used in this project.

Control – For transferring control information. Examples include resetting the device, querying information about the device, etc. All USB devices always have the default control endpoint point zero.

Interrupt – For small and fast data transfer, typically of up to 8 bytes. Examples include data 
transfer for serial ports, human interface devices (HIDs) like keyboard, mouse, etc.

Bulk – For big but comparatively slow data transfer. A typical example is data transfers for mass storage devices.

Isochronous – For big data transfer with bandwidth guarantee, though data integrity may not be guaranteed. Typical practical usage examples include transfers of time-sensitive data like of audio, video


