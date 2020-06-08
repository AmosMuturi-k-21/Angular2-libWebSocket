#pragma once
#ifdef __cplusplus
extern "C" {
#endif

	void init_AD7714(void);

	void ecrit_AD7714_8(char);
	char lit_AD7714_8(void);
	void lit_AD7714(union ptr_sp, char taille);

	void conv_pret(char voie);
	char conv_test(char voie);

	unsigned long acqui_can(char);

	void init_voie(char);


#ifdef __cplusplus
} // closing brace for extern "C"
#endif