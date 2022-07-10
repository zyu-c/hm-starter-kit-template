void init_spi_gyro(void);
void init_spi_enc(void);
void preprocess_spi_gyro(int address);
void preprocess_spi_enc(int address);
void write_spdr(void);
int ovre_check(void);
int gyro_read_check(void);
int gyro_write_check(void);
long read_gyro_data(void);

void write_spdr_gyro(void);
void spii_int_gyro(void);
void read_spdr_gyro(void);
void write_spdr_enc(void);
void spii_int_enc(void);
void read_spdr_enc(void);

int get_enc_data(void);
