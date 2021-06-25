#include <stdio.h>
#include <stdint.h>
#include <string.h>

void unsigned_sum (uint32_t *Array, uint64_t *result);
void signed_sum (int *Array, int *result);

int main(void)
{
	uint32_t unsigned_arr[] = {12 , 2, 5, 4, 5};
	int signed_arr[] = {-12 , 2, 5, 4, 5};
	uint64_t un_res;
	int sig_res;
	

	unsigned_sum (unsigned_arr, &un_res);
	signed_sum (signed_arr, &sig_res);
	
	while (1);
}
