package io.emqx.mqtt;

import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;


import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;

public abstract class BaseFragment extends Fragment {
    public FragmentActivity fragmentActivity;

    public BaseFragment() {
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        View view = inflater.inflate(getLayoutResId(), container, false);

        setUpView(view);

        return view;
    }

    protected abstract int getLayoutResId();

    protected abstract void setUpView(View view);


    @Override
    public void onAttach(@NonNull Context context) {
        super.onAttach(context);
        this.fragmentActivity = getActivity();
    }


    @Override
    public void onDestroyView() {
        super.onDestroyView();
    }
}
