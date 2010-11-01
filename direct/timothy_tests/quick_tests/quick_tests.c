#include "quick_tests.h"

#include "yaffsfs.h"
int random_seed;
int simulate_power_failure = 0;



typedef struct test {
	int (*p_function)(void);	/*pointer to test function*/
	void (*p_function_clean)(void);
	/*char pass_message[50]; will not need a pass message*/
	char *fail_message;	/*pointer to fail message, needs to include name of test*/
}test_template;


int dummy_test_pass(void){
	//printf("running dummy_test_pass\n");
	return 1;
}
void dummy_test_pass_clean(void){
	
}
int dummy_test_fail(void){
	//printf("running dummy_test_fail\n");
	return 0;
}
void dummy_test_fail_clean(void){
}
test_template test_list[]={
	{dummy_test_pass,dummy_test_pass_clean,"dummy_test_pass"},
	{dummy_test_fail,dummy_test_fail_clean,"dummy_test_fail"},
	{mount_yaffs_test,mount_yaffs_test_clean,"mount_yaffs_test"}
	};


int main(){
	int output=0;
	unsigned int num_of_tests_pass=0;
	int num_of_tests_failed=0;
	unsigned int total_number_of_tests=(sizeof(test_list)/sizeof(test_template));
	unsigned int x=0;	
	init_quick_tests();
	printf("quick tests for yaffs\n");
	//printf("testing yaffs\n");

	//printf("len function %d",(sizeof(test_list)/sizeof(test_template)));
	for (x=0;x<total_number_of_tests;x++){
		//printf("x %d\n",x);
		output=test_list[x].p_function();	/*run test*/
		if (output>0){
			/*test has passed*/
			num_of_tests_pass++;
		}
		else {
			/*test is assumed to have failed*/
			//printf("test failed\n");
			printf("test: %s failed\n",test_list[x].fail_message);		
			num_of_tests_failed ++;	
		}
		test_list[x].p_function_clean();	/*run test*/
			
	}
	if (num_of_tests_pass==total_number_of_tests){
		printf("\t OK \n");
	}
	printf("tests: %d passed %d failed\n",num_of_tests_pass,num_of_tests_failed);
	return 0;
}

void init_quick_tests(void){
	yaffs_start_up();;
	//yaffs_set_trace(0);

}
