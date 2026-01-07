void loop()
{
    if(t == shrehold)
    {
        matrix_write(0, 0, pwm_mode ? pixel_off : c);
        matrix_flush();
    }
    
    if(t == 0)
    {
        t = T_MAX;
        shrehold++;

        if(shrehold == T_MAX)
        {
            pwm_mode ^= 1;
            shrehold = 0;
        }
        
        matrix_write(0, 0, pwm_mode ? c : pixel_off);
        matrix_flush();
    }

    t--;
}
